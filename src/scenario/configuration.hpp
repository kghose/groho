/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains declarations for the scenario configuration data structure
*/
#pragma once

#include <optional>
#include <set>
#include <string>
#include <unordered_map>

#include "scenariolib.hpp"

namespace sim {

struct Configuration {

    std::string name     = "No name";
    double      begin_jd = 2458248.5;
    double      end_jd   = 2458249.5;
    double      step     = 60;

    std::set<std::string> orrery_fnames;
    std::set<std::string> flightplan_fnames;

    typedef std::string sst;

    bool set_key_value(std::optional<KeyValue> kv)
    {
        std::unordered_map<sst, std::function<void(sst)>> keyword_map{

            { "name", [=](sst v) { name      = v; } },
            { "begin", [=](sst v) { begin_jd = stof(v); } },
            { "end", [=](sst v) { end_jd     = stof(v); } },
            { "step", [=](sst v) { step      = stof(v); } },
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

std::optional<Configuration> parse_configuration(std::string fname);
}