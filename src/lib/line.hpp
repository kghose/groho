/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

Small structures that help us decouple parameters for simulation objects from
the objects themselves. This allows us to, e.g. load parameters from a file
without having to have knowledge of the objects they represent
*/

#pragma once

#include <filesystem>
#include <string>

namespace groho {

namespace fs = std::filesystem;

struct ParseStatus {
    enum StatusCode { PENDING, OK, WARNING, ERROR };
    StatusCode  code = StatusCode::PENDING;
    std::string message;
};

struct Line {
    fs::path    file_path;
    size_t      line;
    std::string key;
    std::string value;
    ParseStatus status;

    // Good enough for MVP
    bool operator!=(const Line& rhs)
    {
        return !(
            (line == rhs.line) && (key == rhs.key) && (value == rhs.value));
    }
};

inline void
add_issue(Line* line_p, ParseStatus::StatusCode code, std::string issue)
{
    if (line_p != nullptr) {
        line_p->status.code = line_p->status.code == ParseStatus::ERROR
            ? ParseStatus::ERROR
            : code;
        line_p->status.message += " " + issue;
    }
}

typedef std::vector<Line> Lines;

}
