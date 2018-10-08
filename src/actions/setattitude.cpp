/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Set attitude of ship
*/

#include "flightplanaction.hpp"

namespace sim {

struct SET_ATTITUDE : public FlightPlanAction {

    Vector att;

    void setup(
        [[maybe_unused]] SnapShot<ShipLike>&                    this_ship,
        [[maybe_unused]] const Collection<SnapShotV<RockLike>>& system)
    {
        ;
    }

    ShipCommand execute(
        const SnapShot<ShipLike>&                               this_ship,
        [[maybe_unused]] const Collection<SnapShotV<RockLike>>& system)
    {
        DLOG_S(INFO) << this_ship.property.naif.name << " attitude set";

        done = true;
        return { {}, att, {} };
    }

    bool is_blocking() const { return false; }

    std::string usage() const
    {
        return R"(Set ship attitude:
    x:1.1 y:0.4 z:0.2)";
    }
};

template <>
std::unique_ptr<FlightPlanAction>
construct<SET_ATTITUDE>(params_t* params, std::ifstream* ifs)
{
    auto action = std::unique_ptr<SET_ATTITUDE>(new SET_ATTITUDE());

    if (ifs) {
        // code to load from file
        return action;
    }

    if (params) {
        try {

            action->att = Vector{ stof((*params)["x"]),
                                  stof((*params)["y"]),
                                  stof((*params)["z"]) };
            return action;

        } catch (std::exception& e) {

            LOG_S(ERROR) << "Usage error:\n" << action->usage();
            return {};
        }
    }

    return {};
}
}