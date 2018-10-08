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
            if (*burn_end_time < this_ship.state.t_s) {
                return { acc, {}, {} };
            } else {
                done = true;
                return { 0, {}, {} };
            }
        } else {
            burn_end_time = this_ship.state.t_s + burn_duration;
            if (acc > this_ship.property.max_acc) {
                LOG_S(WARNING)
                    << line_no << ": SET_ACCEL for "
                    << this_ship.property.naif.name << " exceeds max_acc";
                acc = this_ship.property.max_acc;
            }
            return { acc, {}, {} };
        }
    }

    bool is_blocking() const { return false; }

    std::string usage() const
    {
        return R"(burn: Fire engines for given time.
    
usage:
    burn acc:0.01 for:10)";
    }
};

template <>
std::unique_ptr<FlightPlanAction>
construct<BURN>(params_t* params, std::ifstream* ifs)
{
    auto action = std::unique_ptr<BURN>(new BURN());

    if (ifs) {
        // code to load from file
        return action;
    }

    if (params) {
        try {
            action->acc           = stof((*params)["acc"]);
            action->burn_duration = stod((*params)["for"]);
            return action;
        } catch (std::exception& e) {
            LOG_S(ERROR) << "Usage error:\n" << action->usage();
            return {};
        }
    }

    return {};
}
}