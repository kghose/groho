/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Store bare information from flightplan file.
*/

#pragma once

#include <string>
#include <vector>

#include "parseerror.hpp"
#include "units.hpp"

namespace groho {

struct PlanElement {
    J2000_s                  t;
    std::string              command;
    std::vector<std::string> arguments;
    size_t                   line;
};

struct FlightPlan {
    std::string              name;
    PlanElement              initial_condition;
    std::vector<PlanElement> route;
    std::string              path;
    std::vector<ParseError>  errors;
};

FlightPlan load_flight_plan(const std::string& path);
}