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

#include "flightplanaction.hpp"
#include "scenariolib.hpp"
#include "vector.hpp"

namespace sim {

enum FlightState { FALLING, LANDED };

struct FlightPlan {

    std::string name = "No name";
    double      max_acc;
    double      max_fuel;
    double      fuel_cons_rate;
    FlightState flight_state;
    Vector      position;
    Vector      velocity;

    // TODO: How to handle landed state and landed position in initial
    // conditions

    typedef std::string sst;

    bool set_key_value(std::optional<KeyValue> kv)
    {
        std::unordered_map<sst, std::function<void(sst)>> keyword_map{

            { "name", [=](sst v) { name                     = v; } },
            { "max-acceleration", [=](sst v) { max_acc      = stof(v); } },
            { "max-fuel", [=](sst v) { max_fuel             = stof(v); } },
            { "fuel-cons-rate", [=](sst v) { fuel_cons_rate = stof(v); } },
            { "flight-state", [=](sst v) { flight_state     = FALLING; } },
            // TODO: handle landed state
            { "position", [=](sst v) { position             = stoV(v); } },
            { "velocity", [=](sst v) { velocity             = stoV(v); } }

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

std::optional<FlightPlan> parse_flight_plan(std::string fname);
}