/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Park a spacecraft around a given body
*/

#include "flightplanaction.hpp"

namespace sim {

// Utility to compute deltaV given GM, R and V
Vector parking_deltaV(double GM, Vector R, Vector Vs)
{
    double r_mod       = R.norm();
    Vector minus_R_hat = R.normed() * -1;
    Vector Vs_hat      = Vs.normed();
    Vector Uo_hat      = cross(minus_R_hat, Vs_hat);
    Vector Vo_hat      = cross(Uo_hat, minus_R_hat);
    Vector Vo          = Vo_hat * std::sqrt(GM / r_mod);
    return Vo - Vs;
}

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

    spkid_t target;
    size_t  target_idx;

    bool parking_maneuver_started = false;

    PARK_IN_ORBIT(const FPAmeta& _meta)
        : FlightPlanAction(_meta)
    {
    }

    void setup(State& state)
    {
        if (!set_body_idx(state.orrery, target, target_idx)) {
            done = true;
            return;
        }
    }

    void operator()(State& state)
    {
        auto const& ob   = state.orrery[target_idx];
        auto&       ship = state.ships[meta.ship_idx];

        // If this is the first time we call this we need to gather more state
        // information
        if (!last_t_s) {
            last_t_s = ship.state.t;
            return;
        }

        Vector R = ship.state.pos - ob.state.pos;
        if (R.norm() > R_capture) {
            return;
        }

        if (!parking_maneuver_started) {
            DLOG_S(INFO) << meta.fname << ": Line " << meta.line_no
                         << ": Starting to park " << ship.property.name
                         << " -> " << ob.property.name;
            parking_maneuver_started = true;
        }

        Vector deltaV = parking_deltaV(
            ob.property.GM, ship.state.pos - ob.state.pos, ship.state.vel);

        ship.state.att = deltaV.normed();
        // deltaV = 0.5 * A * Tstep^2
        double Tstep = state.t_s - *last_t_s;
        *last_t_s    = state.t_s;
        double acc   = deltaV.norm() / (0.5 * Tstep * Tstep);

        if (acc > ship.param.max_acc) {
            ship.state.acc = ship.param.max_acc;
            LOG_S(WARNING)
                << meta.fname << ": Line " << meta.line_no
                << ": acceleration required for parking exceeds max_acc";
        } else {
            ship.state.acc = acc;
        }

        if (deltaV.norm() < epsilon) {
            done = true;
            LOG_S(INFO) << meta.fname << ": Line " << meta.line_no << ": Ship "
                        << ship.property.name << " successfully parked around "
                        << ob.property.name;
        }
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
    } catch (std::exception) {
        LOG_S(ERROR) << _meta.fname << ": Line: " << _meta.line_no
                     << ": Park in orbit needs two elements eg: id:399 r:200";
        return {};
    }
}
}
