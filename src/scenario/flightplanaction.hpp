/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains forward declarations for the verb/noun combinations
available to control spacecraft via the flight plans.
*/
#pragma once

#include <forward_list>
#include <memory>
#include <optional>
#include <string>

#include "body.hpp"
#include "state.hpp"

namespace sim {

// These are actions spacecraft can be scripted to do
struct FlightPlanAction {

    virtual void operator()(State&) = 0;

    size_t      ship_idx;
    std::string fname;
    size_t      line_no;
    double      t_s;
    bool        done = false;
};

typedef std::unique_ptr<FlightPlanAction> fpa_uptr_t;
typedef std::forward_list<fpa_uptr_t>     fpa_uptr_l_t;

inline bool fpa_order(const fpa_uptr_t& a, const fpa_uptr_t& b)
{
    return a->t_s < b->t_s;
}

fpa_uptr_t parse_line_into_action(
    size_t ship_idx, std::string fname, size_t line_no, std::string line);
}