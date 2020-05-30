/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file defines the simulator code
*/
#include <filesystem>

#include "simulation.hpp"
#include "simulator.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

Simulator::Simulator(std::string scn_file, std::string sim_folder)
{
    auto lines = load_input_file(scn_file);
    if (!lines) {
        return;
    }

    Scenario   scenario(*lines);
    Simulation simulation(scenario);

    double    dt = 60;
    v3d_vec_t orrery_pos;

    orrery_pos.resize(simulation.orrery.list_bodies().size());

    for (double t = scenario.begin; t < scenario.end; t += dt) {
        simulation.orrery.set_to(t, orrery_pos);
        simulation.solar_system.append(orrery_pos);
    }
}

}