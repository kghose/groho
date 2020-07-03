/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Utility functions for parsing input strings
*/
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace groho {

const std::string wspace = " \t\n\r\f\v";

inline std::string trim_whitespace(const std::string& line)
{
    size_t start = line.find_first_not_of(wspace);
    if (start == std::string::npos) {
        return "";
    }

    size_t stop = line.find_last_not_of(wspace);
    if (stop == std::string::npos) {
        stop = line.length();
    }

    return line.substr(start, stop + 1);
}

inline std::vector<std::string>
split_string(const std::string& s, const std::string& sep = wspace)
{
    std::vector<std::string> tokens;

    size_t start = 0, stop = 0;
    for (;;) {
        start = s.find_first_not_of(sep, stop);
        if (start == std::string::npos)
            break;
        stop = std::min(s.find_first_of(sep, start), s.length());
        tokens.push_back(trim_whitespace(s.substr(start, stop - start)));
    }
    return tokens;
}

class Parameters {
public:
    Parameters(std::vector<std::string> tokens)
    {
        for (const auto& token : tokens) {
            const auto parts = split_string(token, ":");
            params[parts[0]] = parts[1];
        }
    }

    std::string get(std::string key, std::string def = "")
    {
        auto idx = params.find(key);
        return idx != params.end() ? idx->second : def;
    }

private:
    std::unordered_map<std::string, std::string> params;
};

}