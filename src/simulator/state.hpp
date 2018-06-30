/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This class collects Orrery, Spaceships and other object state at a given instant
*/

#pragma once

#include <vector>

#include "body.hpp"

namespace sim {

struct State {

    State(
        double                   _t_s,
        const std::vector<Body>& _orrery,
        std::vector<Body>&       _ships)
        : t_s(_t_s)
        , orrery(_orrery)
        , ships(_ships)
    {
    }

    double t_s;

    // This points to the state inside the Orrery itself
    const std::vector<Body>& orrery;

    // This points to the ship structure in the Scenario
    std::vector<Body>& ships;
};
} // sim