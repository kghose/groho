/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Park a spacecraft around a given body
*/

#include "actionlib.hpp"
#include "flightplanaction.hpp"

namespace sim {

// V0.0
// Once we come within R km of a planet, maneuver to set V to match orbital VO
// For each step
// 1. Compute the velocity vector required to convert V to VO (deltaV)
// 2. Set attitude along deltaV
// 3. Set engine accel (A) such that deltaV = 0.5 * A * Tstep^2 or maxA
// 4. Once |V| is within epsilon (e) of |VO| deactivate this program
struct PARK_IN_ORBIT : public FlightPlanAction {

    const double epsilon = 0.001; // 1m/s tolerance
    double       R_capture;

    std::optional<double> last_t_s;

    NAIFbody target;
    size_t   target_idx;

    bool parking_maneuver_started = false;

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

        // If this is the first time we call this we need to gather more state
        // information
        if (!last_t_s) {
            last_t_s = this_ship.state.t_s;
            return {};
        }

        Vector R = this_ship.state.pos - target_body.pos();
        if (R.norm() > R_capture) {
            return {};
        }

        if (!parking_maneuver_started) {
            DLOG_S(INFO) << flightplan_fname << ": Line " << line_no
                         << ": Starting to park "
                         << this_ship.property.naif.name << " -> "
                         << target_body.property.naif.name;
            parking_maneuver_started = true;
        }

        Vector deltaV = parking_deltaV(
            target_body.property.GM,
            this_ship.state.pos - target_body.pos(),
            this_ship.state.vel);

        ShipCommand cmd;
        cmd.att = deltaV.normed();
        // deltaV = 0.5 * A * Tstep^2
        double Tstep = this_ship.state.t_s - *last_t_s;
        *last_t_s    = this_ship.state.t_s;
        double acc   = deltaV.norm() / (0.5 * Tstep * Tstep);

        if (acc > this_ship.property.max_acc) {
            cmd.acc = this_ship.property.max_acc;
            LOG_S(WARNING)
                << flightplan_fname << ": Line " << line_no
                << ": acceleration required for parking exceeds max_acc";
        } else {
            cmd.acc = acc;
        }

        if (deltaV.norm() < epsilon) {
            done = true;
            LOG_S(INFO) << flightplan_fname << ": Line " << line_no << ": Ship "
                        << this_ship.property.naif.name
                        << " successfully parked around "
                        << target_body.property.naif.name;
        }

        return cmd;
    }

    bool is_blocking() const { return true; }

    std::string usage() const
    {
        return R"(Park in orbit
    id:399 \ ; NAIF id of target body  
    r:200    ; altitude for parking )";
    }
};

template <>
std::unique_ptr<FlightPlanAction>
construct<PARK_IN_ORBIT>(params_t* params, std::ifstream* ifs)
{
    auto action = std::unique_ptr<PARK_IN_ORBIT>(new PARK_IN_ORBIT());

    if (ifs) {
        // code to load from file
        return action;
    }

    if (params) {

        try {
            action->target    = stof((*params)["id"]);
            action->R_capture = stof((*params)["r"]);
            return action;
        } catch (std::exception& e) {
            throw "Usage error:\n" + action->usage();
        }
    }

    return {};
}
}
