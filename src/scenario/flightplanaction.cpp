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
#include <unordered_map>

#include "body.hpp"
#include "flightplanaction.hpp"
#include "groholib.hpp"
#include "orrery.hpp"
#include "scenariolib.hpp"
#include "state.hpp"
#include "vector.hpp"

namespace sim {

// Utility functions /////////////////////////////////////////////////////////

bool FlightPlanAction::set_body_idx(
    const std::vector<Body>& bodies, spkid_t spkid, size_t& idx)
{
    auto _idx = body_index(bodies, spkid);
    if (!_idx) {
        LOG_S(ERROR) << meta.fname << ":" << meta.line_no << ": "
                     << meta.command_string << ": Can't find " << spkid.id;
        return false;
    }
    idx = *_idx;
    return true;
}

///////////////////////////////
// AVAILABLE ACTIONS (VERBS) //
///////////////////////////////

struct SET_ATTITUDE;
struct SET_ACCEL;
struct INITIAL_ORBIT;
struct PARK_IN_ORBIT;

std::unordered_map<str_t, std::function<fpap_t(const FPAmeta&, params_t&)>>
    available_actions{ { "set-attitude", construct<SET_ATTITUDE> },
                       { "set-accel", construct<SET_ACCEL> },
                       { "initial-orbit", construct<INITIAL_ORBIT> },
                       { "park", construct<PARK_IN_ORBIT> } };

fpap_t parse_line_into_action(
    size_t                   ship_idx,
    std::string              fname,
    size_t                   line_no,
    std::vector<std::string> tokens)
{
    double jd = do_this_first;
    if (tokens[0] != "-") {
        jd = stod(tokens[0]);
    }

    if (available_actions.count(tokens[1])) {
        params_t params;

        for (size_t i = 2; i < tokens.size(); i++) {
            auto kv = get_named_parameter(tokens[i]);
            if (kv) {
                params[kv->key] = kv->value;
            }
        }

        return available_actions[tokens[1]](
            FPAmeta{ ship_idx, fname, line_no, tokens[1], jd2s(jd) }, params);

    } else {
        LOG_S(ERROR) << fname << ": " << line_no
                     << ": Unknown action: " << tokens[1];
        return {};
    }
}
}