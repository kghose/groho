/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This declares the simulation scenario data structure
*/
#pragma once

#include <forward_list>
#include <optional>
#include <string>
#include <vector>

#include "body.hpp"
#include "configuration.hpp"
#include "flightplanaction.hpp"
#include "spkorrery.hpp"

namespace sim {

struct Scenario {

    // We want to construct a scenario and then have the simulator iterate on it
    // While it is cheap to load configurations and flightplans, orreries can
    // be expensive to both load and keep around, so we have
    void from(const Configuration&);

    Configuration     config;
    orrery::SpkOrrery orrery; // the BodyState is set internally
    std::vector<Body> ships;  // the BodyState has to be set explicitly
    fpapl_t           actions;

    // TODO: more fine grained errors?
    bool valid;
};
}