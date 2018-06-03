/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains declarations for the scenario Ship data structure.
The code takes care of parsing flight plans from file, representing
them as lists of actions.
*/
#pragma once

#include <cmath>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>

#include "body.hpp"
#include "flightplanaction.hpp"
#include "scenariolib.hpp"
#include "vector.hpp"

namespace sim {

struct Ship {

    Body body;

    std::vector<std::shared_ptr<FlightPlanAction>> plan;

    // Initialization data that requires world state and can only be done
    // once we start running the simulator
    std::string initial_orbit;

    // TODO: How to handle landed state and landed position in initial
    // conditions

    typedef std::string sst;

    bool set_key_value(std::optional<KeyValue> kv)
    {
        std::unordered_map<sst, std::function<void(sst)>> keyword_map{

            { "name", [=](sst v) { body.name                = v; } },
            { "max-acceleration", [=](sst v) { body.max_acc = stof(v); } },
            { "max-fuel", [=](sst v) { body.max_fuel        = stof(v); } },
            { "fuel-cons-rate", [=](sst v) { body.fuel_cons = stof(v); } },
            { "orbiting", [=](sst v) { initial_orbit        = v; } },
            { "flight-state",
              [=](sst v) { body.state.flight_state          = FALLING; } },
            // TODO: handle landed state
            { "position", [=](sst v) { body.state.pos       = stoV(v); } },
            { "velocity", [=](sst v) { body.state.vel       = stoV(v); } },
            { "attitude", [=](sst v) { body.state.att       = stoV(v); } },
            //
            { "fuel", [=](sst v) { body.state.fuel          = stof(v); } },
            { "acc", [=](sst v) { body.state.acc            = stof(v); } },

        };

        if (keyword_map.count(kv->key)) {
            keyword_map[kv->key](kv->value);
            return true;
        } else {
            return false;
        }
    };

    // Any initialization that requires actual world state and can only be done
    // after we start running the simulator.
    // This expects the Orrery to be computed with velocities
    void init(const WorldState& ws)
    {
        if (initial_orbit.length() > 0) {
            set_initial_state_as_orbiting(ws);
        }
    }

    void set_initial_state_as_orbiting(const WorldState& ws);
};

bool        operator==(const Ship& a, const Ship& b);
inline bool operator!=(const Ship& a, const Ship& b) { return !(a == b); }

std::optional<Ship> parse_flight_plan(std::string fname, int dssc);
}