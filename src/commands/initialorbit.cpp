/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Implements the initial orbiting state for spaceships
*/

#include "initialorbit.hpp"
#include "parsing.hpp"

namespace groho {

void set_initial_orbit(
    double    GM,
    double    r1,
    double    r2,
    const V3d Pbody,
    const V3d Vbody,
    V3d&      pos,
    V3d&      vel)
{
    V3d U_hat = cross(Vbody, Pbody * -1).normed();
    V3d V_hat = cross(Pbody * -1, U_hat).normed();
    pos       = Pbody + (Pbody.normed() * r1);
    vel       = (V_hat * std::sqrt(2 * GM * r2 / (r1 * (r1 + r2)))) + Vbody;
}

void set_initial_orbit(
    const CommandToken& command, const State& state, V3d& pos, V3d& vel)
{
    // No error checking for now
    NAIFbody center     = std::stoi(command.params[0]);
    auto     components = split_string(command.params[1], "x");
    double   a1         = std::stod(components[0]);
    double   a2         = std::stod(components[1]);

    size_t idx = state.orrery.idx_of(center);
    set_initial_orbit(
        state.orrery.body(idx).GM,
        state.orrery.body(idx).r + a1,
        state.orrery.body(idx).r + a2,
        state.orrery.pos(idx),
        state.orrery.vel(idx),
        pos,
        vel);
}

}