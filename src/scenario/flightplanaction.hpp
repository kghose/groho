/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains forward declarations for the verb/noun combinations
available to control spacecraft via the flight plans.
*/
#pragma once

#include <optional>

#include "vector.hpp"

namespace sim {

// These are actions spacecraft can be scripted to do
enum Verb : unsigned int;

// Each noun has one of the datatypes listed here, including complex data types
// as needed
union Noun {
    Vector vector;
    double fraction;
    Noun() { fraction = 0.0; }
};

// An action consists of a date, a verb and a noun
struct FlightPlanAction {
    double      jd;
    Verb        verb;
    Noun        noun;
    std::string help;
};

std::optional<FlightPlanAction> parse_line_into_action(std::string line);
}