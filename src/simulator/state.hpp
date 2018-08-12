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
    State(
        const std::vector<RockLike::Property> _rocks,
        const std::vector<ShipLike::Property> _ships)
    {
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
        system.N = 1 - system.N;
        _t_s[system.N] += d_t_s;
    }

    // TODO: rename this to "solar_system" and "fleet"
    // std::vector<SnapShot<RockLike>>& system() { return _rocks[N]; }
    // std::vector<SnapShot<ShipLike>>& fleet() { return _ships; }

    // constexpr const std::vector<SnapShot<RockLike>>& system() const
    // {
    //     return _rocks[N];
    // }
    // constexpr const std::vector<SnapShot<ShipLike>>& fleet() const
    // {
    //     return _ships;
    // }

    Collection<RockSnapShotWithVel> system;
    Collection<SnapShot<ShipLike>>  fleet;

private:
    double _t_s[2];
};
} // sim