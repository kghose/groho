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

struct ParseStatus {
    enum StatusCode { OK = 0, WARNING, ERROR };
    size_t      line;
    StatusCode  code;
    std::string message;
};

template <typename T> struct Parsed {
    T           value;
    ParseStatus status;
    Parsed() {}
    Parsed(const T value, const ParseStatus status)
        : value(value)
        , status(status)
    {
    }
};
}
