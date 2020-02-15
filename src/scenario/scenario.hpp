/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Store bare information from Scenario file.
*/

#pragma once

#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#include "naifbody.hpp"
#include "parsestatus.hpp"
#include "units.hpp"

namespace groho {

namespace fs = std::filesystem;

typedef Parsed<fs::path> PathParam;
typedef Parsed<NAIFbody> NAIFParam;

struct Scenario {
    std::string name;
    J2000_s     begin;
    J2000_s     end;

    std::vector<PathParam> orrery_files; // Order matters
    std::vector<NAIFParam> include_set;
    std::vector<PathParam> ship_files;

    fs::path path; // all linked files are relative to this

    std::vector<ParseStatus> issues;
};

Scenario load_scenario(const std::string& path);
}