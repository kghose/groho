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

Simulator::Simulator(std::string scn_file, std::string outdir)
{
    auto lines = load_input_file(scn_file);
    if (!lines) {
        return;
    }

    Scenario   scenario(*lines);
    Simulation simulation(scenario, outdir);

    v3d_vec_t orrery_pos;

    orrery_pos.resize(simulation.orrery.list_bodies().size());

    const auto& sim = scenario.sim;
    LOG_S(INFO) << "start: " << sim.begin.as_ut();
    LOG_S(INFO) << "end:   " << sim.end.as_ut();
    LOG_S(INFO) << "step:  " << sim.dt;
    for (double t = sim.begin; t < sim.end; t += sim.dt) {
        simulation.orrery.set_to(t, orrery_pos);
        simulation.solar_system.append(orrery_pos);
    }
}

}