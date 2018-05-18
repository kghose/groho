/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This declares the simulation scenario data structure
*/
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "configuration.hpp"
#include "flightplan.hpp"

namespace sim {

struct Scenario {

    Scenario() { ; }
    Scenario(std::string fname_);

    std::string fname;

    std::optional<Configuration> configuration;
    std::vector<FlightPlan>      flight_plans;
};

bool        operator==(const Scenario& a, const Scenario& b);
inline bool operator!=(const Scenario& a, const Scenario& b)
{
    return !(a == b);
}
}