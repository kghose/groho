/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This defines some functions for the simulation scenario data structure
*/
#include <iostream>
#include <set>

#include "scenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

bool time_range_extended(
    const orrery::SpkOrrery& orrery, const Configuration& config)
{
    return (config.begin_s < orrery.begin_s) || (orrery.end_s < config.end_s);
}

bool orrery_changed(const Configuration& a, const Configuration& b)
{
    return (a.orrery_fnames != b.orrery_fnames);
}

void Scenario::from(const Configuration& new_config)
{
    bool reload_orrery = (config.orrery_fnames != new_config.orrery_fnames)
        || ((new_config.begin_s < orrery.begin_s)
            || (orrery.end_s < new_config.end_s));

    if (reload_orrery) {
        orrery = orrery::SpkOrrery(new_config.begin_s, new_config.end_s);
        for (auto orrery_name : new_config.orrery_fnames) {
            orrery.load_orrery_model(orrery_name);
        }
    }

    ships.clear();
    int default_ship_code = -1000;
    for (auto& fp_name : new_config.flightplan_fnames) {
        auto ship = parse_ship_properties(fp_name /*, --default_ship_code*/);
        if (ship)
            ships.push_back(*ship);
    }

    config = new_config;
    valid  = true;
}

} // namespace sim