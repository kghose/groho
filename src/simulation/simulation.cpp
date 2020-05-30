/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#include "simulation.hpp"

namespace groho {

Simulation::Simulation(const Scenario& scenario_)
{
    set_from_new_scenario(scenario_);
}

void Simulation::set_from_new_scenario(const Scenario& scenario_)
{
    // Hardcoded for prototyping
    double   dt   = 60;
    fs::path path = "output";

    // For our first implementation, we don't do any work reuse
    scenario = scenario_;

    orrery       = Orrery(scenario.begin, scenario.end, scenario.kernel_tokens);
    solar_system = Serialize(dt, orrery.list_bodies(), path);
    spacecraft   = Serialize(dt, {}, path);
}

}