/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/
#pragma once

#include <filesystem>

#include "orrery.hpp"
#include "scenario.hpp"
#include "serialize.hpp"
#include "simparams.hpp"

namespace groho {

namespace fs = std::filesystem;

struct State {
    v3d_vec_t orrery_pos;
    v3d_vec_t ship_pos;
    v3d_vec_t ship_vel;
    v3d_vec_t ship_acc;
};

struct Simulation {

    Simulation(const Scenario& scenario, const fs::path& outdir);

    Scenario  scenario;
    Orrery    orrery;
    Serialize solar_system, spacecraft;

    State state;

    void
    set_from_new_scenario(const Scenario& scenario, const fs::path& outdir);

    bool requires_state_initialization() { return true; }
};

}