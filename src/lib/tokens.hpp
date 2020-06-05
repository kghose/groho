/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

Small structures that help us decouple parameters for simulation objects from
the objects themselves. This allows us to, e.g. load parameters from a file
without having to have knowledge of the objects they represent
*/

#pragma once

#include <filesystem>
#include <unordered_map>
#include <unordered_set>

#include "line.hpp"
#include "naifbody.hpp"
#include "units.hpp"

namespace fs = std::filesystem;

namespace groho {

struct KernelToken {
    std::unordered_set<NAIFbody> codes;
    fs::path                     path;

    Line *pick_line_p, path_line_p;
};

typedef std::vector<KernelToken> KernelTokens;

struct CommandToken {
    J2000_s                  start;
    double                   duration;
    std::string              command;
    std::vector<std::string> params;

    Line* line_p;
};

typedef std::vector<CommandToken> CommandTokens;

struct SpacecraftToken {
    NAIFbody      code;
    std::string   craft_name;
    CommandToken  initial_condition;
    CommandTokens command_tokens;

    Line* line_p;
};

typedef std::vector<SpacecraftToken> SpacecraftTokens;

}