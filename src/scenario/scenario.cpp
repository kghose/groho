/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This defines some functions for the simulation scenario data structure
*/
#include <iostream>
#include <set>

#include "scenario.hpp"
//#include "tokenizedscenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

Scenario::Scenario(std::string fname_)
{
    fname = fname_;

    configuration = parse_configuration(fname);
    if (configuration) {
        int default_ship_code = -1000;
        for (auto& fp_name : configuration->flightplan_fnames) {
            auto fp = parse_flight_plan(fp_name, --default_ship_code);
            if (fp)
                flight_plans.push_back(*fp);
        }
    }
}

time_t Scenario::latest_modification()
{
    std::vector<std::string> fnames;
    fnames.push_back(fname);
    fnames.insert(
        fnames.end(),
        configuration->orrery_fnames.begin(),
        configuration->orrery_fnames.end());
    fnames.insert(
        fnames.end(),
        configuration->flightplan_fnames.begin(),
        configuration->flightplan_fnames.end());
    return file_modification_time(fnames);
}

bool operator==(const Scenario& a, const Scenario& b)
{
    if (*a.configuration != *b.configuration)
        return false;
    return true;
}

} // namespace sim