/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/
#pragma once

#include <string>

#include "orrery.hpp"
#include "scenario.hpp"
#include "serialize.hpp"

namespace groho {

struct Simulation {

    Simulation(const Scenario& scenario);

    Scenario  scenario;
    Orrery    orrery;
    Serialize solar_system, spacecraft;

    void set_from_new_scenario(const Scenario& scenario);
};

}