/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

Some utilities for parsing input files.
*/
#pragma once

#include <string>
#include <vector>

namespace groho {

struct Line {
    size_t      line;
    std::string key;
    std::string value;
};

typedef std::vector<Line> Lines;

class InputFile {
public:
    InputFile(const std::string& path);
    Lines load() const;

private:
    std::string path;
};

std::vector<std::string> split_string(const std::string s);
}
