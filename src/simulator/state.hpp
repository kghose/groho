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
    State(const Objects<SnapShot> snapshot)
    {
        _rocks[0] = snapshot.system;
        _rocks[1] = snapshot.system;
        _ships    = snapshot.fleet;
    }

    void compute_rock_vels()
    {
        const double delta_t_s = _t_s[N] - _t_s[1 - N];
        for (size_t i = 0; i < _rocks[N].size(); i++) {
            _rocks[N][i].state.vel
                = (_rocks[N][i].state.pos - _rocks[1 - N][i].state.pos)
                / delta_t_s;
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
    std::vector<SnapShot<RockLike>>& system() { return _rocks[N]; }
    std::vector<SnapShot<ShipLike>>& fleet() { return _ships; }

    constexpr const std::vector<SnapShot<RockLike>>& system() const
    {
        return _rocks[N];
    }
    constexpr const std::vector<SnapShot<ShipLike>>& fleet() const
    {
        return _ships;
    }

private:
    std::vector<SnapShot<RockLike>> _rocks[2];
    double                          _t_s[2];
    std::vector<SnapShot<ShipLike>> _ships;

    int N;
};
} // sim