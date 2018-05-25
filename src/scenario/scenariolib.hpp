/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains some common functions used in parsing scenario files
*/
#pragma once

#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

// For file modification time
#include <sys/stat.h>
#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

#include "vector.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

const double T0        = 2451545.0;
const double S_PER_DAY = 86400.0;

inline double jd2s(double jd) { return (jd - T0) * S_PER_DAY; }

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

inline std::vector<std::string> split(std::string line)
{
    std::istringstream iss(line);
    return std::vector<std::string>(
        std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>());
}

inline Vector stoV(std::string v)
{
    std::vector<std::string> tokens = split(v);
    if (tokens.size() < 3) {
        LOG_S(ERROR) << "Vector needs three components: Got " << v;
        return {};
    }
    Vector out{ stof(tokens[0]), stof(tokens[1]), stof(tokens[2]) };
    return out;
}

inline time_t file_modification_time(std::string fname)
{
    struct stat result;
    if (stat(fname.c_str(), &result) == 0) {
        return result.st_mtime;
    } else {
        return 0;
    }
}

// Given a list of files, give us the time of latest change among them all
inline time_t file_modification_time(std::vector<std::string> fnames)
{
    time_t fmod_time = 0;
    for (auto& fn : fnames) {
        time_t fn_ft = file_modification_time(fn);
        if (fn_ft > fmod_time) {
            fmod_time = fn_ft;
        }
    }
    return fmod_time;
}
}