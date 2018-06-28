/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Start ship out in orbit
*/

#include "flightplanaction.hpp"

namespace sim {

struct INITIAL_ORBIT : public FlightPlanAction {

    INITIAL_ORBIT(const FPAmeta& _meta)
        : FlightPlanAction(_meta)
    {
    }

    void setup(State& state)
    {
        if (!set_body_idx(state.orrery, origin, B_idx)) {
            done = true;
            return;
        }

        auto const& ob   = state.orrery[B_idx];
        auto&       ship = state.ships[meta.ship_idx];

        double A     = alt;
        double r     = ob.property.r;
        Vector Rsun  = ob.state.pos;
        Vector Vbody = ob.state.vel;
        Vector U_hat = cross(Vbody, Rsun * -1).normed();
        Vector V_hat = cross(Rsun * -1, U_hat).normed();
        ship.state.pos
            = state.orrery[B_idx].state.pos + (Rsun.normed() * (A + r));
        ship.state.vel
            = (V_hat * std::sqrt(state.orrery[B_idx].property.GM / (A + r)))
            + Vbody;
        ship.state.att = V_hat; // Nice to set this too

        ship.state.flight_state = FALLING;

        done = true;
    }

    void operator()(State& state) {}

    spkid_t origin;
    double  alt;
    size_t  B_idx;
};

template <>
fpap_t construct<INITIAL_ORBIT>(const FPAmeta& _meta, params_t& params)
{
    if (_meta.t_s > do_this_first) {
        LOG_S(ERROR) << _meta.fname << ": Line: " << _meta.line_no
                     << ": initial orbit action can only be defined to "
                        "occur at start of simulation. Time must be '-'";
    }

    try {
        auto action    = ptr_t<INITIAL_ORBIT>(new INITIAL_ORBIT(_meta));
        action->origin = stoi(params["id"]);
        action->alt    = stod(params["alt"]);
        return action;
    } catch (std::exception) {
        LOG_S(ERROR)
            << _meta.fname << ": Line: " << _meta.line_no
            << ": Need two elements for initial orbit: id:399 alt:3000";
        return {};
    }
}
}