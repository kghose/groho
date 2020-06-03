/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#include "simulation.hpp"

namespace groho {

Simulation::Simulation(const Scenario& scenario_, const fs::path& outdir)
{
    set_from_new_scenario(scenario_, outdir);
}

void Simulation::set_from_new_scenario(
    const Scenario& scenario_, const fs::path& outdir)
{
    // For our first implementation, we don't do any work reuse
    scenario = scenario_;

    orrery
        = Orrery(scenario.sim.begin, scenario.sim.end, scenario.kernel_tokens);
    orrery_objects = orrery.list_objects();
    grav_index     = orrery.index_of_gravitational_objects();

    std::vector<NAIFbody> oo_naifs;
    for (const auto& oo : orrery_objects) {
        oo_naifs.push_back(oo.code);
    }
    solar_system = Serialize(scenario.sim, oo_naifs, outdir);
    spacecraft   = Serialize(scenario.sim, {}, outdir);
}

}