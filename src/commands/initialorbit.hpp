/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/
#pragma once

#include "state.hpp"
#include "tokens.hpp"
#include "v3d.hpp"

namespace groho {

void set_initial_orbit(
    const CommandToken& command, const State& state, V3d& pos, V3d& vel);

}