/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Base action class and base parser for flight plan actions
*/

#pragma once

#include <list>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "body.hpp"
#include "state.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

// These are pleasant to alias
// typedef std::string                      str_t;
// typedef std::vector<std::string>         strv_t;
typedef std::unordered_map<std::string, std::string> params_t;

const double do_this_first = std::numeric_limits<double>::lowest();

// These are the actions we can set
struct ShipCommand {
    std::optional<float>       acc;
    std::optional<Vector>      att;
    std::optional<std::string> signal;
};

// All actions are derived from this base class
struct FlightPlanAction {

    // This is like a type name, which allows us to deserialize this thing
    // properly
    std::string        _action_name;
    std::string        flightplan_fname;
    size_t             line_no;
    std::string        command_string;
    ShipLike::Property property;

    // Set this flag once the action is completely done and can be discarded
    bool done = false;

    // derived classes add state here ...

    // http://www.gotw.ca/publications/mill18.htm Guideline #4
    virtual ~FlightPlanAction() = default;

    // Any setup that has to be done at start of simulation. This typically
    // involves resolving indexes into the orrery that can only
    // be known after the whole scenario has been loaded
    virtual void setup(
        SnapShot<ShipLike>&                    this_ship,
        const Collection<SnapShotV<RockLike>>& system)
        = 0;

    // Given the state of the universe, do any computations we need and then
    // craft an action to perform
    virtual ShipCommand execute(
        const SnapShot<ShipLike>&              this_ship,
        const Collection<SnapShotV<RockLike>>& system)
        = 0;

    // This is true for blocking actions. This prevents the simulator from
    // processing the next actions until this one is done and removed
    virtual bool is_blocking() const = 0;

    // Return a helpful usage string
    virtual std::string usage() const = 0;

    // We have to convert an id to an index into a list of bodies very often
    // If we fail, we have to disable the action and warn the user. It is
    // pleasant to wrap the usual find function with all this.
    template <typename T>
    size_t find(const std::vector<T>& bodies, const NAIFbody& body)
    {
        auto _idx = sim::find(bodies, body);
        if (!_idx) {
            std::string msg = flightplan_fname + ":" + std::to_string(line_no)
                + ": " + command_string + ": Can't find "
                + std::to_string((int)body.code);
            throw std::runtime_error(msg);
        }
        return *_idx;
    }
};

// One ship's flight plan
typedef std::list<std::unique_ptr<FlightPlanAction>> FlightPlan;

// All the flight plans of the fleet
typedef std::vector<FlightPlan> FlightPlans;

template <typename T>
std::unique_ptr<FlightPlanAction> construct(params_t* params);

// Each FPA we add specializes this function template
// We can initialize an action either from a parameter dictionary or from a
// file containing a saved state (which will be used for restarts)
template <typename T>
std::unique_ptr<FlightPlanAction>
try_construct(std::string name, params_t* params)
{
    // We'd handle the null pointer case (we just want the action constructed
    // because we are reading it's data from disk) here, but templating will not
    // allow us to have std::unique_ptr<T>(new T()) for forward declarations
    try {
        auto action          = construct<T>(params);
        action->_action_name = name; // ensures we serialize it
        return action;
    } catch (std::exception& e) {
        LOG_S(ERROR) << "Usage error:\n" << construct<T>(nullptr)->usage();
        return {};
    }
}

std::unique_ptr<FlightPlanAction> parse_line_into_action(std::string line);

std::string list_available_actions();
}