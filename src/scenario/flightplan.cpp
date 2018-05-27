/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains definitions for the scenario flightplan data structure.
The code takes care of parsing flight plans from file, representing
them as lists of actions.
*/

#include <fstream>

#include "flightplan.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

std::optional<FlightPlan> parse_flight_plan(std::string fname, int default_code)
{

    std::ifstream cfile(fname);
    int           line_no = 0;
    std::string   line;
    FlightPlan    flt;
    bool          header_section = true;

    if (!cfile) {
        LOG_S(ERROR) << fname << ": Flight plan not found";
        return {};
    }

    // Some miscellaneous defaults
    flt.body.body_type = SPACESHIP;
    flt.body.color     = 0xffffff;
    flt.body.code      = default_code;

    while (std::getline(cfile, line)) {
        line_no++;

        line = trim_whitespace(trim_comments(line));

        if (line.length() == 0) // Ignore empty lines
            continue;

        if (header_section) {
            std::optional<KeyValue> kv = get_key_value(line);
            if (kv) {
                if (kv->key == "plan") {
                    header_section = false;
                } else {
                    if (!flt.set_key_value(kv)) {
                        LOG_S(WARNING) << "Line " << line_no << ": Unknown key "
                                       << kv->key << ": ignoring";
                    }
                }
            } else {
                LOG_S(ERROR)
                    << "Error parsing line: " << line_no << " (" << line << ")";
            }
        } else {
            auto fpa = parse_line_into_action(line, line_no);
            if (fpa) {
                flt.plan.push_back(fpa);
            }
        }
    }
    return flt;
}
}