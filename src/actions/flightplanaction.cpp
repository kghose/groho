/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Base action class and base parser for flight plan actions
*/

#include <cctype>
#include <cmath>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

#include "body.hpp"
#include "flightplanaction.hpp"
#include "groholib.hpp"
#include "scenariolib.hpp"
#include "state.hpp"
#include "vector.hpp"

namespace sim {

// These are pleasant to alias
typedef std::string                      str_t;
typedef std::vector<std::string>         strv_t;
typedef std::unordered_map<str_t, str_t> params_t;

typedef FlightPlanAction fpa_t;
template <class T> using ptr_t = std::unique_ptr<T>;
typedef ptr_t<FlightPlanAction> fpap_t;
typedef std::list<fpap_t>       fpapl_t;

///////////////////////////////
// AVAILABLE ACTIONS (VERBS) //
///////////////////////////////

struct INITIAL_STATE;
struct SET_ATTITUDE;
struct BURN;
struct INITIAL_ORBIT;
struct PARK_IN_ORBIT;
struct ORIENT;
struct PHASE;
struct WAITTILL;

std::map<
    std::string,
    std::function<std::unique_ptr<FlightPlanAction>(std::string, params_t*)>>
    available_actions{ { "initial-state", try_construct<INITIAL_STATE> },
                       { "set-attitude", try_construct<SET_ATTITUDE> },
                       { "burn", try_construct<BURN> },
                       { "initial-orbit", try_construct<INITIAL_ORBIT> },
                       { "park", try_construct<PARK_IN_ORBIT> },
                       { "orient", try_construct<ORIENT> },
                       { "wait-till-phase", try_construct<PHASE> },
                       { "wait-till", try_construct<WAITTILL> } };

std::unique_ptr<FlightPlanAction> parse_line_into_action(std::string line)
{
    auto tokens = split(line);

    if (available_actions.count(tokens[0])) {

        params_t params;
        for (size_t i = 1; i < tokens.size(); i++) {
            auto kv = get_named_parameter(tokens[i]);
            if (kv) {
                params[kv->key] = kv->value;
            }
        }

        try {
            return available_actions[tokens[0]](tokens[0], &params);
        } catch (std::exception& e) {
            throw std::domain_error(
                "Can't parse action: " + line + "\n" + e.what());
        }

    } else {
        throw std::invalid_argument("Unknown action: " + line);
    }
}

std::string list_available_actions()
{
    std::string list_str = "Available actions:\n";
    for (auto const& [name, func] : available_actions) {
        list_str += name + "  \\ ; " + func(name, nullptr)->usage() + "\n\n";
    }
    return list_str;
}
}