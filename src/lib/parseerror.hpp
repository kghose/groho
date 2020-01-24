/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

We parse most strings in input files to some other object, such as a date or a
flight plan action. This parse may result in errors. The error is returned in
this structure.
*/
#pragma once

#include <string>

namespace groho {

struct ParseError {
    bool        error;
    size_t      line;
    std::string message;
};
}
