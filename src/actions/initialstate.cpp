/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Set initial state of ship
*/

#include "flightplanaction.hpp"

namespace sim {

struct INITIAL_STATE : public FlightPlanAction {

    std::optional<Vector> pos;
    std::optional<Vector> vel;
    std::optional<Vector> att;
    std::optional<double> acc;
    std::optional<double> fuel;

    void setup(
        SnapShot<ShipLike>&                                     this_ship,
        [[maybe_unused]] const Collection<SnapShotV<RockLike>>& system)
    {
        if (pos)
            this_ship.state.pos = *pos;
        if (vel)
            this_ship.state.vel = *vel;
        if (att)
            this_ship.state.att = *att;
        if (acc)
            this_ship.state.acc = *acc;
        if (fuel)
            this_ship.state.fuel = *fuel;

        done = true;
    }

    ShipCommand execute(
        [[maybe_unused]] const SnapShot<ShipLike>&              this_ship,
        [[maybe_unused]] const Collection<SnapShotV<RockLike>>& system)
    {
        return {};
    }

    bool is_blocking() const { return false; }

    std::string usage() const
    {
        return R"(Set initial state for space ship
    ; none or more of the following optional params to be specified
    px:1.0 py:1.0 pz:1.0 \ ; raw position (km)
    vx:1.0 vy:1.0 vz:1.0 \ ; raw velocity (km/s)
    ax:1.0 ay:1.0 az:1.0 \ ; raw attitute
    acc:0.01 \ ; acceleration (km/s^2)
    fuel:0.5   ; fuel level (ratio)
    )";
    }
};

template <>
std::unique_ptr<FlightPlanAction> construct<INITIAL_STATE>(params_t* params)
{
    auto action = std::unique_ptr<INITIAL_STATE>(new INITIAL_STATE());

    if (params) {
        if ((*params).count("px")) {
            action->pos = Vector{ stof((*params)["px"]),
                                  stof((*params)["py"]),
                                  stof((*params)["pz"]) };
        }

        if ((*params).count("vx")) {
            action->vel = Vector{ stof((*params)["vx"]),
                                  stof((*params)["vy"]),
                                  stof((*params)["vz"]) };
        }

        if ((*params).count("ax")) {
            action->att = Vector{ stof((*params)["ax"]),
                                  stof((*params)["ay"]),
                                  stof((*params)["az"]) };
        }

        if ((*params).count("acc")) {
            action->acc = stod((*params)["acc"]);
        }

        if ((*params).count("fuel")) {
            action->fuel = stod((*params)["fuel"]);
        }
    }

    return action;
}
}