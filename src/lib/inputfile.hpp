/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

This file contains some common functions used in parsing input files
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
}
