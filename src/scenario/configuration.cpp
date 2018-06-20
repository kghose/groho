/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains definitions for some functions to load and parse the scenario
file into the configuration data structure
*/

#include <fstream>
#include <vector>

#include "configuration.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

time_t Configuration::latest_modification()
{
    std::vector<std::string> fnames;
    fnames.push_back(fname);
    fnames.insert(fnames.end(), orrery_fnames.begin(), orrery_fnames.end());
    fnames.insert(
        fnames.end(), flightplan_fnames.begin(), flightplan_fnames.end());
    return file_modification_time(fnames);
}

Configuration parse_configuration(std::string fname)
{

    std::ifstream cfile(fname);
    int           line_no = 0;
    std::string   line;
    Configuration cfg;

    cfg.fname = fname;

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
    if (a.begin_s != b.begin_s)
        return false;
    if (a.end_s != b.end_s)
        return false;
    if (a.step_s != b.step_s)
        return false;
    if (a.orrery_fnames != b.orrery_fnames)
        return false;
    if (a.flightplan_fnames != b.flightplan_fnames)
        return false;
    return true;
}
}