/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

While the spacecraft vel and accel are computed and set explicitly, only the
position of an orrery body is set. We use a rolling buffer to keep track of this
and enable vel and accel computations.
*/
#pragma once

#include "craftstate.hpp"
#include "orrerystate.hpp"

namespace groho {

struct State {
    State() {}
    State(
        const std::vector<BodyConstant>& bodies,
        const std::vector<size_t>&       grav_body_idx,
        const std::vector<NAIFbody>&     codes,
        double                           dt)
        : orrery(bodies, grav_body_idx, dt)
        , spacecraft(codes)
    {
    }

    OrreryState orrery;
    CraftState  spacecraft;
    double      t;
};

}