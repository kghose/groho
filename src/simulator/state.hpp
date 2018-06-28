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

    State(double t_s, const std::vector<Body>& orrery, std::vector<Body>& ships)
        : t_s(t_s)
        , orrery(orrery)
        , ships(ships)
    {
    }

    double t_s;

    // This points to the state inside the Orrery itself
    const std::vector<Body>& orrery;

    // This points to the ship structure in the Scenario
    std::vector<Body>& ships;
};
} // sim