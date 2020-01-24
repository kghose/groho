/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

Some utilities for parsing input files.
*/

#include <fstream>
#include <iostream>

#include "inputfile.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

InputFile::InputFile(const std::string& path)
    : path(path)
{
}

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

inline std::string trim_comment(const std::string& line)
{
    size_t stop = line.find_first_of(";");
    if (stop == std::string::npos) {
        return line;
    }
    return line.substr(0, stop);
}

void split_key_value(
    const std::string& line, std::string& key, std::string& value)
{
    size_t p = std::min(line.find_first_of(wspace), line.length());
    key      = trim_whitespace(line.substr(0, p));
    value    = trim_whitespace(line.substr(p));
}

Lines InputFile::load() const
{
    Lines         lines;
    std::ifstream cfile(path);
    size_t        line_no = 1;
    std::string   line, key, value;

    if (cfile.fail()) {
        LOG_S(ERROR) << "Input file '" << path << "' not found";
    } else {
        while (std::getline(cfile, line)) {
            line = trim_whitespace(trim_comment(line));
            if (line.length() > 0) {
                split_key_value(line, key, value);
                lines.push_back(Line{ line_no, key, value });
            }
            line_no++;
        }
    }

    return lines;
}

std::vector<std::string> split_string(const std::string s)
{
    std::vector<std::string> tokens;

    size_t start = 0, stop = 0;
    for (;;) {
        start = s.find_first_not_of(wspace, stop);
        if (start == std::string::npos)
            break;
        stop = std::min(s.find_first_of(wspace, start), s.length());
        tokens.push_back(trim_whitespace(s.substr(start, stop - start + 1)));
    }
    return tokens;
}
}