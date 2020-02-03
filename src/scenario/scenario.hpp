/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Store bare information from Scenario file.
*/

#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "naifbody.hpp"
#include "parseerror.hpp"
#include "units.hpp"

namespace groho {

struct Scenario {
    std::string name;
    J2000_s     begin;
    J2000_s     end;

    std::unordered_set<std::string> orrery_files;
    std::unordered_set<NAIFbody>    include_set;
    std::unordered_set<std::string> ship_files;

    std::string path; // all linked files are relative to this

    std::vector<ParseError> errors;
};

Scenario load_scenario(const std::string& path);
}