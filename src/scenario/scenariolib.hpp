/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains some common functions used in parsing scenario files
*/
#pragma once

#include <optional>
#include <string>

namespace sim {

struct KeyValue {
    std::string key, value;
};

const std::string wspace = " \t\n\r\f\v";

inline std::string trim_whitespace(std::string line)
{
    line.erase(0, line.find_first_not_of(wspace));
    line.erase(line.find_last_not_of(wspace) + 1);
    return line;
}

inline std::string trim_comments(std::string line)
{
    return line.erase(std::min(line.find_first_of(';'), line.size()));
}

inline std::optional<KeyValue> get_key_value(std::string line)
{
    KeyValue kv;
    size_t   n0 = line.find_first_of('=');

    if (n0 > line.size()) {
        return {};
    }

    kv.key   = trim_whitespace(line.substr(0, n0 - 1));
    kv.value = trim_whitespace(line.substr(n0 + 1));
    return kv;
}
}