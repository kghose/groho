/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains forward declarations for the verb/noun combinations
available to control spacecraft via the flight plans.
*/
#pragma once

#include <list>
#include <memory>
#include <optional>
#include <string>

#include "body.hpp"
#include "state.hpp"

namespace sim {

struct FPAD {
    size_t      ship_idx;
    std::string fname;
    size_t      line_no;
    double      t_s;
};

// These are actions spacecraft can be scripted to do
struct FlightPlanAction {
    FlightPlanAction(const FPAD& _p)
        : p(_p)
    {
    }
    virtual ~FlightPlanAction()     = default;
    virtual void operator()(State&) = 0;
    const FPAD   p;
    bool         done = false;
};

typedef std::unique_ptr<FlightPlanAction> fpap_t;
typedef std::list<fpap_t>                 fpapl_t;
template <class T> using ptr_t = std::unique_ptr<T>;

inline bool fpa_order(const fpap_t& a, const fpap_t& b)
{
    return a->p.t_s < b->p.t_s;
}

fpap_t parse_line_into_action(
    size_t                   ship_idx,
    std::string              fname,
    size_t                   line_no,
    std::vector<std::string> tokens);
}