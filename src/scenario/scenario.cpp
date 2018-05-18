/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This defines some functions for the simulation scenario data structure
*/
#include <iostream>
#include <set>

#include "scenario.hpp"
//#include "tokenizedscenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

bool operator==(const Scenario& a, const Scenario& b)
{
    if (*a.configuration != *b.configuration)
        return false;
    return true;
}

} // namespace sim