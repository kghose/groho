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
#include "simparams.hpp"
#include "tokens.hpp"
#include "units.hpp"

namespace groho {

namespace fs = std::filesystem;

struct Scenario {

    Scenario() { ; }
    Scenario(const Lines& lines);

    SimParams sim;

    KernelTokens     kernel_tokens;
    SpacecraftTokens spacecraft_tokens;

    Lines lines;

    void parse_preamble(Lines& lines);
    void parse_kernels(Lines& lines);
    void parse_plans(Lines& lines);
    void sort_and_validate_plans();
    void log_issues(const Lines& lines) const;

    bool operator != (const Scenario& rhs);
};
}