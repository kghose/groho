/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains declarations for the scenario flightplan data structure.
The code takes care of parsing flight plans from file, representing
them as lists of actions.
*/
#pragma once

#include <optional>
#include <set>
#include <string>
#include <unordered_map>

#include "body.hpp"
#include "flightplanaction.hpp"
#include "scenariolib.hpp"
#include "vector.hpp"

namespace sim {

struct FlightPlan {

    Body body;

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

            { "flight-state", [=](sst v) { body.flight_state = FALLING; } },
            // TODO: handle landed state
            { "position", [=](sst v) { body.pos              = stoV(v); } },
            { "velocity", [=](sst v) { body.vel              = stoV(v); } },
            { "attitude", [=](sst v) { body.att              = stoV(v); } },
            //
            { "fuel", [=](sst v) { body.fuel                 = stof(v); } },
            { "acc", [=](sst v) { body.acc                   = stof(v); } },

        };

        if (keyword_map.count(kv->key)) {
            keyword_map[kv->key](kv->value);
            return true;
        } else {
            return false;
        }
    };

    std::vector<FlightPlanAction> plan;
};

bool        operator==(const FlightPlan& a, const FlightPlan& b);
inline bool operator!=(const FlightPlan& a, const FlightPlan& b)
{
    return !(a == b);
}

std::optional<FlightPlan> parse_flight_plan(std::string fname, int dssc);
}