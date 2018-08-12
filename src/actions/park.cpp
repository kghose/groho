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

    PARK_IN_ORBIT(const FPAmeta& _meta)
        : FlightPlanAction(_meta)
    {
    }

    void setup(State& state)
    {
        target_idx = find2(state.system.bodies, target);
    }

    ShipCommand execute(const State& state)
    {
        auto const& target_body = state.system[target_idx];
        auto const& ship        = state.fleet[meta.ship_idx];

        // If this is the first time we call this we need to gather more state
        // information
        if (!last_t_s) {
            last_t_s = ship.state.t_s;
            return { {}, {} };
        }

        Vector R = ship.state.pos - target_body.pos();
        if (R.norm() > R_capture) {
            return { {}, {} };
        }

        if (!parking_maneuver_started) {
            DLOG_S(INFO) << meta.fname << ": Line " << meta.line_no
                         << ": Starting to park " << ship.property.naif.name
                         << " -> " << target_body.property.naif.name;
            parking_maneuver_started = true;
        }

        Vector deltaV = parking_deltaV(
            target_body.property.GM,
            ship.state.pos - target_body.pos(),
            ship.state.vel);

        ShipCommand cmd;
        cmd.att = deltaV.normed();
        // deltaV = 0.5 * A * Tstep^2
        double Tstep = state.t_s() - *last_t_s;
        *last_t_s    = state.t_s();
        double acc   = deltaV.norm() / (0.5 * Tstep * Tstep);

        if (acc > ship.property.max_acc) {
            cmd.acc = ship.property.max_acc;
            LOG_S(WARNING)
                << meta.fname << ": Line " << meta.line_no
                << ": acceleration required for parking exceeds max_acc";
        } else {
            cmd.acc = acc;
        }

        if (deltaV.norm() < epsilon) {
            done = true;
            LOG_S(INFO) << meta.fname << ": Line " << meta.line_no << ": Ship "
                        << ship.property.naif.name
                        << " successfully parked around "
                        << target_body.property.naif.name;
        }

        return cmd;
    }
};

template <>
fpap_t construct<PARK_IN_ORBIT>(const FPAmeta& _meta, params_t& params)
{
    try {
        auto action = ptr_t<PARK_IN_ORBIT>(new PARK_IN_ORBIT(_meta));

        action->target    = stof(params["id"]);
        action->R_capture = stof(params["r"]);
        return action;
    } catch (std::exception& e) {
        LOG_S(ERROR) << _meta.fname << ": Line: " << _meta.line_no
                     << ": Park in orbit needs two elements eg: id:399 r:200";
        return {};
    }
}
}
