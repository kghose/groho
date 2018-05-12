/*
  The scenario object holds a parsed version of the scenario files that can


*/
#pragma once

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace sim {

struct KeyValue {
    std::string key, value;
};

struct Configuration {

    std::string name     = "No name";
    double      begin_jd = 2458248.5;
    double      end_jd   = 2458249.5;
    double      step     = 60;

    std::set<std::string> orrery;
    std::set<std::string> flightplan;

    typedef std::string sst;

    bool set_key_value(std::optional<KeyValue> kv)
    {
        std::unordered_map<sst, std::function<void(sst)>> keyword_map{

            { "name", [=](sst v) { name      = v; } },
            { "begin", [=](sst v) { begin_jd = stof(v); } },
            { "end", [=](sst v) { end_jd     = stof(v); } },
            { "step", [=](sst v) { step      = stof(v); } },
            { "orrery", [=](sst v) { orrery.insert(v); } },
            { "flightplan", [=](sst v) { flightplan.insert(v); } }

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

struct Scenario {

    Scenario() { ; }
    Scenario(std::string fname_)
    {
        fname = fname_;

        configuration = parse_configuration(fname);
    }

    std::string fname;

    std::optional<Configuration> configuration;
};

bool        operator==(const Scenario& a, const Scenario& b);
inline bool operator!=(const Scenario& a, const Scenario& b)
{
    return !(a == b);
}
}