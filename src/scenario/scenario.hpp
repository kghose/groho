/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

*/

#pragma once

#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#include "inputfile.hpp"
#include "naifbody.hpp"
#include "orrery.hpp"
#include "units.hpp"

namespace groho {

namespace fs = std::filesystem;

struct Scenario {

    Scenario(Lines& lines);

    J2000_s begin;
    J2000_s end;

    Kernels kernels;

private:
    void parse_preamble(Lines& lines);
    void parse_kernels(Lines& lines);
};
}