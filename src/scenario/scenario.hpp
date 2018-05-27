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
#include "ship.hpp"

namespace sim {

struct Scenario {

    Scenario() { ; }
    Scenario(std::string fname_);

    time_t latest_modification();

    std::string fname;

    std::optional<Configuration> configuration;
    std::vector<Ship>            ships;
};

bool        operator==(const Scenario& a, const Scenario& b);
inline bool operator!=(const Scenario& a, const Scenario& b)
{
    return !(a == b);
}
}