/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Timed burn - set acceleration and the unset it after a predetermined time
*/

#include "flightplanaction.hpp"

namespace sim {

// TODO: What happens when burns are fractions of a time step?
struct BURN : public FlightPlanAction {

    double                acc;
    double                burn_duration;
    std::optional<double> burn_end_time;

    void setup(
        [[maybe_unused]] SnapShot<ShipLike>&                    this_ship,
        [[maybe_unused]] const Collection<SnapShotV<RockLike>>& system)
    {
        ;
    }

    ShipCommand execute(
        const SnapShot<ShipLike>&                               this_ship,
        [[maybe_unused]] const Collection<SnapShotV<RockLike>>& system)
    {
        if (burn_end_time) {
            if (this_ship.state.t_s < *burn_end_time) {
                return { acc, {}, {} };
            } else {
                done = true;
                return { 0,
                         {},
                         Event{ this_ship.state.t_s,
                                { Thing{ this_ship.property.naif,
                                         this_ship.state.pos,
                                         this_ship.state.vel },
                                  {} },
                                Event::BURN_END,
                                "Burn end" } };
            }
        } else {
            burn_end_time = this_ship.state.t_s + burn_duration;
            if (acc > this_ship.property.max_acc) {
                LOG_S(WARNING)
                    << line_no << ": SET_ACCEL for "
                    << this_ship.property.naif.name << " exceeds max_acc";
                acc = this_ship.property.max_acc;
            }
            return { acc,
                     {},
                     Event{ this_ship.state.t_s,
                            { Thing{ this_ship.property.naif,
                                     this_ship.state.pos,
                                     this_ship.state.vel },
                              {} },
                            Event::BURN_START,
                            "Burn start" } };
        }
    }

    bool is_blocking() const { return false; }

    std::string usage() const
    {
        return R"(Fire engines for given time.
    acc:0.01 \ ; acceleration in km/s^2
    for:10  ; duration in s)";
    }
};

template <> std::unique_ptr<FlightPlanAction> construct<BURN>(params_t* params)
{
    auto action = std::unique_ptr<BURN>(new BURN());
    if (params) {
        action->acc           = stof((*params)["acc"]);
        action->burn_duration = stod((*params)["for"]);
    }
    return action;
}
}