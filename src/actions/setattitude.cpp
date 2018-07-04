/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Set attitude of ship
*/

#include "flightplanaction.hpp"

namespace sim {

struct SET_ATTITUDE : public FlightPlanAction {

    SET_ATTITUDE(const FPAmeta& _meta)
        : FlightPlanAction(_meta)
    {
    }

    void setup([[maybe_unused]] State& state) {}

    void operator()(State& state)
    {
        if (state.t_s < meta.t_s)
            return;

        state.ships[meta.ship_idx].state.att = att;
        DLOG_S(INFO) << state.ships[meta.ship_idx].property.name
                     << " attitude set";
        done = true;
    }

    Vector att;
};

template <>
fpap_t construct<SET_ATTITUDE>(const FPAmeta& _meta, params_t& params)
{
    try {
        auto action = ptr_t<SET_ATTITUDE>(new SET_ATTITUDE(_meta));
        action->att
            = Vector{ stof(params["x"]), stof(params["y"]), stof(params["z"]) };

        return action;
    } catch (std::exception& e) {
        LOG_S(ERROR) << _meta.fname << ": Line: " << _meta.line_no
                     << ": Need three floats for attitude vector: eg: "
                        "x:1.1 y:0.4 z:0.2";
        return {};
    }
}
}