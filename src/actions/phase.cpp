/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Block until apropriate phase in orbit has been achieved
*/

#include "actionlib.hpp"
#include "flightplanaction.hpp"

namespace sim {

struct PHASE : public FlightPlanAction {

    NAIFbody                target;
    size_t                  target_idx;
    double                  target_phase;
    static constexpr double phase_tolerance = 1e-3;

    std::string usage() const
    {
        return R"(Block until apropriate phase in orbit has been achieved
    id:399 \    ; NAIF id of target body  
    phase:0 ; Wait till ship is in opposition 
    
    phase:180 ; ship is in conjunction

    phase:90 ; ship is in western quadrature)";
    }

    void setup(
        [[maybe_unused]] SnapShot<ShipLike>&   this_ship,
        const Collection<SnapShotV<RockLike>>& system)
    {
        target_idx = find(system.bodies, target);
    }

    ShipCommand execute(
        const SnapShot<ShipLike>&              this_ship,
        const Collection<SnapShotV<RockLike>>& system)
    {
        auto const& target_body = system[target_idx];

        Vector R0 = target_body.pos();
        Vector R1 = this_ship.state.pos - R0;

        R0 = R0.normed();
        R1 = R1.normed();

        double phase = std::atan2(cross(R0, R1).norm(), dot(R0, R1));

        if (std::fabs(phase - target_phase) < phase_tolerance) {
            done = true;
        }

        return {};
    }

    bool is_blocking() const { return true; }
};

template <> std::unique_ptr<FlightPlanAction> construct<PHASE>(params_t* params)
{
    auto action = std::unique_ptr<PHASE>(new PHASE());
    if (params) {
        action->target       = stof((*params)["id"]);
        action->target_phase = stod((*params)["phase"]) * M_PI / 180.0;
    }
    return action;
}
}
