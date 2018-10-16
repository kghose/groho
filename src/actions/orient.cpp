/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Orient spacecraft relative to orbital plane.
*/

#include "actionlib.hpp"
#include "flightplanaction.hpp"

namespace sim {

struct ORIENT : public FlightPlanAction {

    NAIFbody target;
    size_t   target_idx;
    double   yaw;
    double   pitch;

    std::string usage() const
    {
        return R"(Orient relative to orbit
    id:399 \    ; NAIF id of target body  
    yaw:180 \   ; Angle (degrees right or left) perpendicular to the orbit 
    pitch:0     ; Angle (degrees up or down) in the plane of the orbit

    Examples:

    yaw:0   ; Orient us pro-grade
    pitch:0 ;
    
    yaw:90  ; Orient us South-wards on a west-to-east orbit
    pitch:0 ;

    yaw:0     ; Orient perpendicularly up from orbit tangent
    pitch:90  ; This is different from perpendicularly up from surface if the
              ; orbit is elliptic

    yaw:180   ; orient retrograde and down
    pitch:-45 ; )";
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

        Vector R = this_ship.state.pos - target_body.pos();

        Vector X = (this_ship.state.vel - target_body.vel()).normed();
        Vector Y = cross(R, X).normed();
        Vector Z = cross(X, Y).normed();
        // Now we have a coordinate frame

        ShipCommand cmd;
        cmd.att = X * cos(pitch) * cos(yaw) + Y * cos(pitch) * sin(yaw)
            + Z * sin(pitch);

        done = true;

        return cmd;
    }

    bool is_blocking() const { return true; }
};

template <>
std::unique_ptr<FlightPlanAction> construct<ORIENT>(params_t* params)
{
    auto action = std::unique_ptr<ORIENT>(new ORIENT());
    if (params) {
        action->target = stof((*params)["id"]);
        action->yaw    = stod((*params)["yaw"]) * M_PI / 180.0;
        action->pitch  = stod((*params)["pitch"]) * M_PI / 180.0;
    }
    return action;
}
}
