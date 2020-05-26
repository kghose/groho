/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

Small structures that help us decouple parameters for simulation objects from
the objects themselves. This allows us to, e.g. load parameters from a file
without having to have knowledge of the objects they represent
*/

#pragma once

#include <filesystem>
#include <unordered_set>

#include "naifbody.hpp"

namespace fs = std::filesystem;

namespace groho {

struct Kernel {
    std::unordered_set<NAIFbody> codes;
    fs::path                     path;
};

typedef std::vector<Kernel> Kernels;

}