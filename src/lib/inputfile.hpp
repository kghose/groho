/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

Some utilities for parsing input files.
*/
#pragma once

#include <optional>
#include <string>
#include <vector>

namespace groho {

struct ParseStatus {
    enum StatusCode { OK = 0, WARNING, ERROR };
    StatusCode  code = StatusCode::OK;
    std::string message;
};

struct Line {
    std::string file_name;
    size_t      line;
    std::string key;
    std::string value;
    ParseStatus status;
};

typedef std::vector<Line> Lines;

std::optional<Lines> load_input_file(const std::string& path);

const std::string wspace = " \t\n\r\f\v";

std::vector<std::string>
split_string(const std::string& s, const std::string& sep = wspace);
}
