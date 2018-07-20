/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains declarations for the scenario configuration data structure
*/
#pragma once

#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>

#include "groholib.hpp"
#include "scenariolib.hpp"

namespace sim {

struct Configuration {

    std::string name    = "No name";
    double      begin_s = 2458248.5;
    double      end_s   = 2458249.5;
    double      step_s  = 60;

    std::string           fname;
    std::set<std::string> orrery_fnames;
    std::set<std::string> flightplan_fnames;

    time_t latest_modification();

    typedef std::string sst;

    bool set_key_value(std::optional<KeyValue> kv)
    {
        std::unordered_map<sst, std::function<void(sst)>> keyword_map{

            { "name", [=](sst v) { name     = v; } },
            { "begin", [=](sst v) { begin_s = jd2s(stof(v)); } },
            { "end", [=](sst v) { end_s     = jd2s(stof(v)); } },
            { "step", [=](sst v) { step_s   = stof(v); } },
            { "orrery", [=](sst v) { orrery_fnames.insert(v); } },
            { "flightplan", [=](sst v) { flightplan_fnames.insert(v); } }

        };

        if (keyword_map.count(kv->key)) {
            keyword_map[kv->key](kv->value);
            return true;
        } else {
            return false;
        }
    };
};

bool        operator==(const Configuration& a, const Configuration& b);
inline bool operator!=(const Configuration& a, const Configuration& b)
{
    return !(a == b);
}

Configuration parse_configuration(std::string fname);
}