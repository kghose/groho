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

#include "body.hpp"
#include "state.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

const double do_this_first = std::numeric_limits<double>::lowest();

struct FlightPlanAction;

// These are pleasant to alias
typedef std::string                      str_t;
typedef std::vector<std::string>         strv_t;
typedef std::unordered_map<str_t, str_t> params_t;

typedef FlightPlanAction fpa_t;
template <class T> using ptr_t = std::unique_ptr<T>;
typedef ptr_t<FlightPlanAction> fpap_t;
typedef std::list<fpap_t>       fpapl_t;

// (Meta)data all FPAs have in common
struct FPAmeta {
    size_t ship_idx;
    str_t  fname;
    size_t line_no;
    str_t  command_string;

    ShipLike::Property property;

    double t_s; // This is data. You say tomato, I say tomato
};

// These are the two ship parameters we can set
struct ShipCommand {
    std::optional<float>  acc;
    std::optional<Vector> att;
};

// All actions are derived from this base class
struct FlightPlanAction {
    FlightPlanAction(const FPAmeta& _meta)
        : meta(_meta)
    {
    }

    // http://www.gotw.ca/publications/mill18.htm Guideline #4
    virtual ~FlightPlanAction() = default;

    // Any setup that has to be done at start of simulation. This typically
    // involves resolving indexes into the orrery and ship list that can only
    // be known after the whole scenario has been loaded
    virtual void setup(State&) = 0;

    // Given the state of the universe, do any computations we need and then
    // update the ship's acceleration and attitude as needed
    virtual ShipCommand execute(const State&) = 0;

    // We have to convert an id to an index into a list of bodies very often
    // If we fail, we have to disable the action and warn the user. It is
    // pleasant to wrap the usual find function with all this.
    template <typename T> size_t find2(const std::vector<T>&, const NAIFbody&);
    // TODO: figure out why naming this "find" causes a compiler problems.
    // It identifies "find" as the member function but when I try
    // using global scope but it refuses to recognize. Could not replicate in
    // isolation. For some reason the templated "find" in body.hpp causes
    // problems

    const FPAmeta meta;

    // Set this flag once the action is completely done and can be discarded
    bool done = false;
};

// Each FPA we add specializes this function template
template <class T> fpap_t construct(const FPAmeta& _meta, params_t& params);

// Allows us to sort actions by time of activation
inline bool fpa_order(const fpap_t& a, const fpap_t& b)
{
    return a->meta.t_s < b->meta.t_s;
}

fpap_t parse_line_into_action(
    size_t                   ship_idx,
    std::string              fname,
    size_t                   line_no,
    std::vector<std::string> tokens,
    ShipLike::Property       property);
}