/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Timed burn - set acceleration and the unset it after a predetermined time
*/

#include "flightplanaction.hpp"

namespace sim {

// TODO: What happens when burns are fractions of a time step?
struct BURN : public FlightPlanAction {

    BURN(const FPAmeta& _meta)
        : FlightPlanAction(_meta)
    {
    }

    void setup(State& state) { ; }

    ShipCommand execute(const State& state)
    {
        auto const& ship = state.fleet()[meta.ship_idx];

        if (!burning) {
            burning    = true;
            float _acc = acc;
            if (_acc > ship.property.max_acc) {
                _acc = ship.property.max_acc;
                LOG_S(WARNING) << meta.line_no << ": SET_ACCEL for "
                               << ship.property.naif.name << " exceeds max_acc";
            }

            DLOG_S(INFO) << ship.property.naif.name << " acceleration set";

            return { _acc, {} };

        } else {

            if (state.t_s() < burn_end_time) {
                return {};

            } else {

                done = true;
                return { 0, {} };
            }
        }
    }

    double acc;
    double burn_end_time;
    bool   burning = false;
};

template <> fpap_t construct<BURN>(const FPAmeta& _meta, params_t& params)
{
    try {
        auto action = ptr_t<BURN>(new BURN(_meta));
        action->acc = stof(params["acc"]);

        action->burn_end_time = _meta.t_s + stod(params["for"]);
        return action;
    } catch (std::exception& e) {
        LOG_S(ERROR) << _meta.fname << ": Line: " << _meta.line_no
                     << ": burn command should be like: burn acc:0.01 for:10";
        return {};
    }
}
}