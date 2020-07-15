/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

This file defines the simulator code
*/
#include <filesystem>

#include "commands.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

struct Burn : public Command {

    static std::string usage()
    {
        return R"(
    burn: Orient relative to orbit around central body and burn

    example:
    burn center:399 acc:10 yaw:180 pitch:0

    parameters:
    center:<..> - NAIF id of target body  
    acc:10  - Acceleration m/s^2
    yaw:0   - Angle (degrees right or left) perpendicular to the orbit plane
    pitch:0 - Angle (degrees up or down) in the plane of the orbit
)";
    }

    Burn(const CommandToken& token, const State& state, size_t self_idx)
        : Command(token)
        , self_idx(self_idx)
    {
        auto params = Parameters(token.params);

        target_idx
            = state.orrery.idx_of(std::stoi(params.get("center", "399")));
        acc_val = std::stod(params.get("acc", "10")) / 1000.0;
        yaw     = std::stod(params.get("yaw", "0"));
        pitch   = std::stod(params.get("pitch", "0"));
    }

    V3d execute(const State& state)
    {
        R = state.spacecraft.pos[self_idx] - state.orrery.pos(target_idx);
        X = (state.spacecraft.vel[self_idx] - state.orrery.vel(target_idx))
                .normed();
        Y = cross(R, X).normed();
        Z = cross(X, Y).normed();
        // Now we have a coordinate frame

        return (X * cos(pitch) * cos(yaw) + Y * cos(pitch) * sin(yaw)
                + Z * sin(pitch))
            * acc_val;
    }

private:
    double acc_val;
    size_t self_idx;
    size_t target_idx;
    double yaw;
    double pitch;

    V3d R, X, Y, Z;
};

}