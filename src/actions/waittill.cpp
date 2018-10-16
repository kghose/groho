/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Block until given time passes
*/

#include "actionlib.hpp"
#include "flightplanaction.hpp"
#include "groholib.hpp"

namespace sim {

struct WAITTILL : public FlightPlanAction {

    double t_0;

    std::string usage() const
    {
        return R"(Block rest of program until given JD
    jd:2458248.5  ; Time in Julian Date)";
    }

    void setup(
        [[maybe_unused]] SnapShot<ShipLike>&                    this_ship,
        [[maybe_unused]] const Collection<SnapShotV<RockLike>>& system)
    {
    }

    ShipCommand execute(
        const SnapShot<ShipLike>&                               this_ship,
        [[maybe_unused]] const Collection<SnapShotV<RockLike>>& system)
    {
        if (this_ship.state.t_s < t_0) {
            return {};
        }

        done = true;
        return {};
    }

    bool is_blocking() const { return true; }
};

template <>
std::unique_ptr<FlightPlanAction> construct<WAITTILL>(params_t* params)
{
    auto action = std::unique_ptr<WAITTILL>(new WAITTILL());
    if (params) {
        action->t_0 = jd2s(stod((*params)["jd"]));
    }
    return action;
}
}