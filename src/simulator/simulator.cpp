/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file defines the simulator code
*/
#include <filesystem>

#include "initialorbit.hpp"
#include "simulation.hpp"
#include "simulator.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

void initialize_ships(Simulation& simulation)
{
    auto& pos = simulation.state.spacecraft.pos();
    auto& vel = simulation.state.spacecraft.vel();
    for (size_t i = 0; i < pos.size(); i++) {
        set_initial_orbit(
            simulation.scenario.spacecraft_tokens[i].initial_condition,
            simulation.state,
            simulation.orrery,
            pos[i],
            vel[i]);
    }
}

void update_ship_state(double dt, State& state)
{
    auto& pos = state.spacecraft.pos();
    auto& vel = state.spacecraft.vel();
    auto& acc = state.spacecraft.acc();

    for (size_t i = 0; i < pos.size(); i++) {
        pos[i] += vel[i] * dt;
        vel[i] += acc[i] * dt;
    }
}

void compute_gravitational_acceleration(
    const State& state, const Orrery& orrery, v3d_vec_t& ship_acc)
{
    const auto& ship_pos   = state.spacecraft.pos();
    const auto& orrery_pos = state.orrery.pos();

    for (size_t i = 0; i < ship_pos.size(); i++) {
        ship_acc[i] = { 0, 0, 0 };
        for (size_t g_idx : orrery.grav_body_idx) {
            auto r = orrery_pos[g_idx] - ship_pos[i];
            auto f = orrery.bodies[g_idx].GM / r.norm_sq();
            ship_acc[i] += r.normed() * f;
        }
    }
}

void add_thrust_to_acceleration(
    const State&            state,
    const Serialize&        solar_system,
    const SpacecraftTokens& spacecraft_tokens,
    v3d_vec_t&              ship_acc)
{
}

Simulator::Simulator(std::string scn_file, std::string outdir)
{
    auto lines = load_input_file(scn_file);
    if (!lines) {
        return;
    }

    Scenario   scenario(*lines);
    Simulation simulation(scenario, outdir);

    const auto& sim = scenario.sim;
    LOG_S(INFO) << "start: " << sim.begin.as_ut();
    LOG_S(INFO) << "end:   " << sim.end.as_ut();
    LOG_S(INFO) << "step:  " << sim.dt;

    auto& state = simulation.state;

    double t = sim.begin;

    if (simulation.requires_state_initialization()) {
        // Warm up
        for (size_t i = 0; i < 4; i++) {
            simulation.orrery.pos_at(t, state.orrery.next_pos());
            simulation.solar_system.append(state.orrery.pos());
            t += sim.dt;
        }

        // Initialize ships state
        initialize_ships(simulation);
    }

    // Main sim
    for (; t < sim.end; t += sim.dt) {
        simulation.orrery.pos_at(t, state.orrery.next_pos());
        update_ship_state(sim.dt, state);
        compute_gravitational_acceleration(
            state, simulation.orrery, state.spacecraft.acc());
        add_thrust_to_acceleration(
            state,
            simulation.solar_system,
            simulation.scenario.spacecraft_tokens,
            state.spacecraft.acc());

        simulation.solar_system.append(state.orrery.pos());
        simulation.spacecraft.append(state.spacecraft.pos());
    }
    simulation.solar_system.flush(state.orrery.pos());
    simulation.spacecraft.flush(state.spacecraft.pos());
}

}