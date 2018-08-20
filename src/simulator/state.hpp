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

class State : public RocksAndShips<SnapShotV, SnapShot> {
public:
    State(
        double                                t_s,
        const std::vector<RockLike::Property> _rocks,
        const std::vector<ShipLike::Property> _ships)
    {
        _t_s[0] = t_s;
        _t_s[1] = t_s;

        for (const auto& r : _rocks) {
            system.push_back(r);
        }

        for (const auto& s : _ships) {
            fleet.push_back({ s, {} });
        }
    }

    double& t_s() { return _t_s[system.N]; }
    double  t_s() const { return _t_s[system.N]; }

    void advance_t_s(double d_t_s)
    {
        system.N       = 1 - system.N;
        _t_s[system.N] = _t_s[1 - system.N] + d_t_s;
    }

private:
    double _t_s[2];
};
} // sim