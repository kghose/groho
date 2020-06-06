/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

A base struct, function signatures and some utility functions for spacecraft
commands.
*/
#pragma once

#include <string>

#include "inputfile.hpp"
#include "naifbody.hpp"

namespace groho {

struct OrbitalCommand {

    OrbitalCommand(std::vector<std::string> params)
    {
        // No error checking for now
        center          = std::stoi(params[0]);
        auto components = split_string(params[1], "x");
        a1              = std::stod(components[0]);
        a2              = std::stod(components[1]);
    }

    NAIFbody center;
    double   a1;
    double   a2;
};

}
