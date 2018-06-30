/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Set accleration of ship
*/

#include "flightplanaction.hpp"

namespace sim {

struct SET_ACCEL : public FlightPlanAction {

    SET_ACCEL(const FPAmeta& _meta)
        : FlightPlanAction(_meta)
    {
    }

    void setup([[maybe_unused]] State& state) {}

    void operator()(State& state)
    {
        if (state.t_s < meta.t_s)
            return;

        double _acc = acc;
        if (_acc > state.ships[meta.ship_idx].param.max_acc) {
            _acc = state.ships[meta.ship_idx].param.max_acc;
            LOG_S(WARNING) << meta.line_no << ": SET_ACCEL for "
                           << state.ships[meta.ship_idx].property.name
                           << " exceeds max_acc";
        }

        state.ships[meta.ship_idx].state.acc = _acc;
        DLOG_S(INFO) << state.ships[meta.ship_idx].property.name
                     << " acceleration set";
        done = true;
    }

    double acc;
};

template <> fpap_t construct<SET_ACCEL>(const FPAmeta& _meta, params_t& params)
{
    try {
        auto action = ptr_t<SET_ACCEL>(new SET_ACCEL(_meta));
        action->acc = stof(params["acc"]);
        return action;
    } catch (std::exception) {
        LOG_S(ERROR) << _meta.fname << ": Line: " << _meta.line_no
                     << ": Need one element for accel eg: acc:23.45";
        return {};
    }
}
}