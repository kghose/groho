/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/
#pragma once

#include "commands.hpp"
#include "orrery.hpp"
#include "state.hpp"
#include "v3d.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

void set_initial_orbit(
    double GM, double r, const V3d Pbody, const V3d Vbody, V3d& pos, V3d& vel)
{
    V3d U_hat = cross(Vbody, Pbody * -1).normed();
    V3d V_hat = cross(Pbody * -1, U_hat).normed();
    pos       = Pbody + (Pbody.normed() * r);
    vel       = (V_hat * std::sqrt(GM / r)) + Vbody;
}

void set_initial_orbit(
    const CommandToken& command,
    const State&        state,
    const Orrery&       orrery,
    V3d&                pos,
    V3d&                vel)
{
    auto orbit = OrbitalCommand(command.params);
    // No error checking for now
    size_t idx = orrery.naif_to_idx.at(orbit.center);
    // Need to handle separate peri and apo-apses
    set_initial_orbit(
        orrery.bodies[idx].GM,
        orrery.bodies[idx].r + orbit.a1,
        state.orrery.pos()[idx],
        state.orrery.vel(idx),
        pos,
        vel);
}

}