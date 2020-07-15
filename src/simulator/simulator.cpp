/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

This file defines the simulator code
*/
#include <chrono>
#include <cstring> // gcc needs this for strerror
#include <filesystem>

#include "commands.hpp"
#include "filelock.hpp"
#include "initialorbit.hpp"
#include "simulation.hpp"
#include "simulator.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

namespace fs = std::filesystem;

Simulator::Simulator(
    std::string scn_file, std::string outdir, bool non_interactive)
    : scn_file(scn_file)
    , outdir(outdir)
{
    keep_looping     = !non_interactive;
    main_loop_thread = std::thread(&Simulator::main_loop, this);
}

void Simulator::main_loop()
{
    for (;;) {
        auto lines = load_input_file(scn_file);
        if (lines) {
            auto new_scenario = Scenario(*lines);
            if (new_scenario != current_scenario) {
                if (sim_thread.joinable()) {
                    keep_running = false;
                    sim_thread.join();
                }
                current_scenario = new_scenario;
                keep_running     = true;
                sim_thread       = std::thread(&Simulator::run, this);
            }
        }
        if (keep_looping) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } else {
            if (sim_thread.joinable()) {
                sim_thread.join();
            }
            break;
        }
    }
}

void Simulator::quit()
{
    keep_running = false;
    keep_looping = false;
    main_loop_thread.join();
}

void initialize_ships(Simulation& simulation);
void velocity_vertlet_pt1(const double dt, State& state);
void compute_gravitational_acceleration(State& state);
void add_thrust_to_acceleration(std::vector<Plan>& plans, State& state);
void velocity_vertlet_pt2(const double dt, State& state);
void save_manifest(const State& state, std::string outdir);

void Simulator::run()
{
    if (!fs::exists(outdir)) {
        fs::create_directories(outdir);
    }

    FileLock lock(outdir);

    Simulation simulation(current_scenario, outdir);

    const auto& sim = current_scenario.sim;
    LOG_S(INFO) << "start: " << sim.begin.as_ut();
    LOG_S(INFO) << "end:   " << sim.end.as_ut();
    LOG_S(INFO) << "step:  " << sim.dt;

    auto& state = simulation.state;

    double t     = sim.begin;
    size_t steps = 0;

    if (simulation.requires_state_initialization()) {
        // Warm up
        for (size_t i = 0; i < 4; i++, steps++) {
            simulation.orrery.pos_at(t, state.orrery.next_pos());
            t += sim.dt;
        }

        // Initialize ships state
        initialize_ships(simulation);
        compute_gravitational_acceleration(state);
    }

    std::vector<Plan> plans;
    for (size_t i = 0; i < simulation.scenario.spacecraft_tokens.size(); i++) {
        plans.emplace_back(simulation.scenario.spacecraft_tokens[i], state, i);
    }

    // Main sim
    for (; t < sim.end && keep_running; t += sim.dt, steps++) {
        state.t = t;
        velocity_vertlet_pt1(sim.dt, state);
        simulation.orrery.pos_at(t, state.orrery.next_pos());
        compute_gravitational_acceleration(state);
        add_thrust_to_acceleration(plans, state);
        velocity_vertlet_pt2(sim.dt, state);

        simulation.solar_system.append(state.orrery.pos());
        simulation.spacecraft.append(state.spacecraft.pos);
    }
    LOG_S(INFO) << steps << " steps";

    save_manifest(state, outdir);
}

void initialize_ships(Simulation& simulation)
{
    auto& pos = simulation.state.spacecraft.pos;
    auto& vel = simulation.state.spacecraft.vel;
    for (size_t i = 0; i < pos.size(); i++) {
        set_initial_orbit(
            simulation.scenario.spacecraft_tokens[i].initial_condition,
            simulation.state,
            pos[i],
            vel[i]);
    }
}

void velocity_vertlet_pt1(const double dt, State& state)
{
    for (size_t i = 0; i < state.spacecraft.pos.size(); i++) {
        state.spacecraft.vel[i] += 0.5 * state.spacecraft.acc[i] * dt;
        state.spacecraft.pos[i] += state.spacecraft.vel[i] * dt;
        state.spacecraft.pos[i].t = state.t;
    }
}

void compute_gravitational_acceleration(State& state)
{
    for (size_t i = 0; i < state.spacecraft.pos.size(); i++) {
        state.spacecraft.acc[i] = { 0, 0, 0 };
        for (size_t g_idx : state.orrery.grav_body_idx()) {
            auto   r     = state.orrery.pos(g_idx) - state.spacecraft.pos[i];
            double r_bar = r.norm();
            auto   f     = state.orrery.body(g_idx).GM / (r_bar * r_bar);
            state.spacecraft.acc[i] += r * (f / r_bar);
        }
    }
}

void add_thrust_to_acceleration(std::vector<Plan>& plans, State& state)
{
    for (size_t i = 0; i < plans.size(); i++) {
        plans[i].execute(state, state.spacecraft.acc[i]);
    }
}

void velocity_vertlet_pt2(const double dt, State& state)
{
    for (size_t i = 0; i < state.spacecraft.pos.size(); i++) {
        state.spacecraft.vel[i] += 0.5 * state.spacecraft.acc[i] * dt;
    }
}

void save_manifest(const State& state, std::string outdir)
{
    // The world's worst YAML serializer
    std::ofstream file(fs::path(outdir) / "manifest.yml", std::ios::out);
    if (file.fail()) {
        LOG_S(ERROR) << std::strerror(errno);
        LOG_S(ERROR) << "Could not write manifest file";
        return;
    }

    file << "time: "
         << std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now())
         << std::endl;

    file << "bodies: \n";
    for (size_t i = 0; i < state.orrery.size(); i++) {
        const auto& body = state.orrery.body(i);
        file << "  " << int(body.code) << ":\n";
        file << "    name: \"" << body.name << "\"\n";
        file << "    r: " << body.r << "\n";
    }
}

}