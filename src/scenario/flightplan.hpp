/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Store bare information from flightplan file.
*/

#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "parsestatus.hpp"
#include "units.hpp"

namespace groho {

namespace fs = std::filesystem;

struct PlanElement {
    J2000_s                  t;
    std::string              command;
    std::vector<std::string> arguments;
};

typedef Parsed<PlanElement> PlanElementParam;

struct FlightPlan {
    std::string                   name;
    PlanElementParam              initial_condition;
    std::vector<PlanElementParam> route;
    fs::path                      path;
    std::vector<ParseStatus>      issues;
};

FlightPlan load_flight_plan(const std::string& path);
}