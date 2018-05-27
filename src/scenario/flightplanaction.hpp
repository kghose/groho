/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains forward declarations for the verb/noun combinations
available to control spacecraft via the flight plans.
*/
#pragma once

#include <optional>

#include "orrery.hpp"
#include "scenariolib.hpp"
#include "vector.hpp"

namespace sim {

struct WorldState {
    WorldState(double t_s, const orrery::OrreryBodyVec& obv)
        : t_s(t_s)
        , obv(obv)
    {
    }

    double                       t_s;
    const orrery::OrreryBodyVec& obv;
};

// These are actions spacecraft can be scripted to do
struct FlightPlanAction {

    FlightPlanAction(double jd, size_t line_no)
        : t_s(jd2s(jd))
        , line_no(line_no)
    {
        active = true;
    }

    virtual void operator()(Body&, const WorldState&) = 0;

    const double t_s;
    const size_t line_no;

    bool active = false;
};

std::shared_ptr<FlightPlanAction>
parse_line_into_action(std::string line, size_t line_no);
}