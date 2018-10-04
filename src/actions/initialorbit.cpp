/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Start ship out in orbit
*/

#include "flightplanaction.hpp"

namespace sim {

struct INITIAL_ORBIT : public FlightPlanAction {

    NAIFbody target;
    double   alt;

    void setup(
        SnapShot<ShipLike>&                    this_ship,
        const Collection<SnapShotV<RockLike>>& system)
    {
        auto target_idx = find(system.bodies, target);

        const auto& target_body = system[target_idx];

        double A            = alt;
        double r            = target_body.property.r;
        Vector Rsun         = target_body.pos();
        Vector Vbody        = target_body.vel();
        Vector U_hat        = cross(Vbody, Rsun * -1).normed();
        Vector V_hat        = cross(Rsun * -1, U_hat).normed();
        this_ship.state.pos = target_body.pos() + (Rsun.normed() * (A + r));
        this_ship.state.vel
            = (V_hat * std::sqrt(target_body.property.GM / (A + r))) + Vbody;
        this_ship.state.att = V_hat; // Nice to set this too

        // ship.state.flight_state = FALLING;

        done = true;
    }

    ShipCommand execute(
        [[maybe_unused]] const SnapShot<ShipLike>&              this_ship,
        [[maybe_unused]] const Collection<SnapShotV<RockLike>>& system)
    {
        return {};
    }
};

template <>
std::unique_ptr<FlightPlanAction>
construct<INITIAL_ORBIT>(params_t* params, std::ifstream* ifs)
{
    auto action = std::unique_ptr<INITIAL_ORBIT>(new INITIAL_ORBIT());

    if (ifs) {
        // code to load from file
        return action;
    }

    if (params) {

        try {
            action->target = NAIFbody(stoi((*params)["id"]));
            action->alt    = stod((*params)["alt"]);
            return action;
        } catch (std::exception& e) {
            LOG_S(ERROR)
                << "Need two elements for initial orbit, e.g.: id:399 alt:3000";
            return {};
        }
    }

    return {};
}
}