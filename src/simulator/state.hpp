/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This class collects Orrery, Spaceships and other object state at a given instant
*/

#pragma once

#include <array>
#include <vector>

#include "buffer.hpp"

namespace sim {

class State {
public:
    State(size_t system_size, size_t fleet_size)
    {
        _rocks[0].resize(system_size);
        _rocks[1].resize(system_size);
        _ships.resize(fleet_size);
    }

    void compute_rock_vels()
    {
        const double delta_t_s = _t_s[N] - _t_s[1 - N];
        for (size_t i = 0; i < _rocks[N].size(); i++) {
            _rocks[N][i].vel
                = (_rocks[N][i].pos - _rocks[1 - N][i].pos) / delta_t_s;
        }
    }

    double& t_s() { return _t_s[N]; }
    double  t_s() const { return _t_s[N]; }

    void advance_t_s(double d_t_s)
    {
        N = 1 - N;
        _t_s[N] += d_t_s;
    }

    // TODO: rename this to "solar_system" and "fleet"
    std::vector<RockLike::State>& system() { return _rocks[N]; }
    std::vector<ShipLike::State>& fleet() { return _ships; }

    constexpr const std::vector<RockLike::State>& system() const
    {
        return _rocks[N];
    }
    constexpr const std::vector<ShipLike::State>& fleet() const
    {
        return _ships;
    }

private:
    std::vector<RockLike::State> _rocks[2];
    double                       _t_s[2];
    std::vector<ShipLike::State> _ships;

    int N;
};
} // sim