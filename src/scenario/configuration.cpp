/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains definitions for some functions to load and parse the scenario
file into the configuration data structure
*/

#include <fstream>

#include "configuration.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

std::optional<Configuration> parse_configuration(std::string fname)
{

    std::ifstream cfile(fname);
    int           line_no = 0;
    std::string   line;
    Configuration cfg;

    // LOG_S(INFO) << "Parsing scenario file " << fname;

    while (std::getline(cfile, line)) {
        line_no++;

        line = trim_whitespace(trim_comments(line));

        if (line.length() == 0) // Ignore empty lines
            continue;

        std::optional<KeyValue> kv = get_key_value(line);
        if (kv) {
            if (!cfg.set_key_value(kv)) {
                LOG_S(WARNING) << "Line " << line_no << ": Unknown key "
                               << kv->key << ": ignoring";
            }
        } else {
            LOG_S(ERROR) << "Error parsing line: " << line_no << " (" << line
                         << ")";
        }
    }
    return cfg;
}

bool operator==(const Configuration& a, const Configuration& b)
{
    if (a.begin_jd != b.begin_jd)
        return false;
    if (a.end_jd != b.end_jd)
        return false;
    if (a.step != b.step)
        return false;
    if (a.orrery != b.orrery)
        return false;
    if (a.flightplan != b.flightplan)
        return false;
    return true;
}
}