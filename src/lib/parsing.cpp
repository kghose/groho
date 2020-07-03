/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Utility functions for parsing input strings
*/

#include "parsing.hpp"

namespace groho {

std::unordered_map<std::string, std::string>
extract_parameters(std::string param_string)
{
    std::unordered_map<std::string, std::string> params;
    for (const auto& token : split_string(param_string, wspace)) {
        const auto parts = split_string(token, ":");
        params[parts[0]] = parts[1];
    }
    return params;
}

}