/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

Small structures that help us decouple parameters for simulation objects from
the objects themselves. This allows us to, e.g. load parameters from a file
without having to have knowledge of the objects they represent
*/

#pragma once

// TODO: rename to tokens.hpp

#include <filesystem>
#include <unordered_map>
#include <unordered_set>

#include "naifbody.hpp"
#include "units.hpp"

namespace fs = std::filesystem;

namespace groho {

struct KernelToken {
    std::unordered_set<NAIFbody> codes;
    fs::path                     path;
};

typedef std::vector<KernelToken> KernelTokens;

struct CommandToken {
    J2000_s                  start;
    double                   duration;
    std::string              plan;
    std::vector<std::string> params;
};

typedef std::vector<CommandToken> CommandTokens;

struct SpacecraftToken {
    std::string   craft_name;
    CommandToken  initial_condition;
    CommandTokens command_tokens;
};

typedef std::unordered_map<std::string, SpacecraftToken> SpacecraftTokens;

}