/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

Small structure to keep track of simulation parameters that need to be passed
around program wide.
*/
#pragma once

#include <filesystem>
#include <iostream>

#include "units.hpp"

namespace fs = std::filesystem;

namespace groho {

struct SimParams {
    J2000_s begin;
    J2000_s end;
    double  dt = 60;
    double  rt = 1.00001;
    double  lt = 1e4;
};

}