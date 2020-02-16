/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Parse scenario input file and return Scenario struct
*/
#include <algorithm>
#include <iostream>

#include "inputfile.hpp"
#include "scenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

bool duplicate(const std::vector<PathParam>& p, const std::string& s)
{
    return std::find_if(
               p.begin(),
               p.end(),
               [s](const PathParam& p) { return p.value == s; })
        != p.end();
}

bool duplicate(const std::vector<NAIFParam>& p, const int c)
{
    return std::find_if(
               p.begin(),
               p.end(),
               [c](const NAIFParam& p) { return p.value == c; })
        != p.end();
}

Scenario::Scenario(const std::string& path)
    : path(path)
{
    set_last_write_time();

    for (auto line : InputFile(path).load()) {

        if (line.key == "name") {
            name = line.value;
            continue;
        }

        if ((line.key == "begin") | (line.key == "end")) {
            auto[date, status] = as_gregorian_date(line.value, line.line);
            if (status.code == ParseStatus::OK) {
                if (line.key == "begin") {
                    begin = date;
                } else {
                    end = date;
                }
            } else {
                issues.push_back(status);
                LOG_S(ERROR)
                    << path << ": L" << status.line << ": " << status.message;
            }
            continue;
        }

        if (line.key == "orrery") {
            if (!duplicate(orrery_files, line.value)) {
                orrery_files.emplace_back(line.value, ParseStatus{ line.line });
            } else {
                ParseStatus err{ line.line,
                                 ParseStatus::WARNING,
                                 "Duplicate orrery" };
                issues.push_back(err);
                LOG_S(ERROR)
                    << path << ": L" << err.line << ": " << err.message;
            }
            continue;
        }

        if (line.key == "ship") {
            if (!duplicate(ship_files, line.value)) {
                ship_files.emplace_back(line.value, ParseStatus{ line.line });
            } else {
                ParseStatus err{ line.line,
                                 ParseStatus::WARNING,
                                 "Duplicate flight plan" };
                issues.push_back(err);
                LOG_S(ERROR)
                    << path << ": L" << err.line << ": " << err.message;
            }
            continue;
        }

        if (line.key == "use-only") {
            for (auto id : split_string(line.value)) {
                int code;
                try {
                    code = std::stoi(id);
                } catch (std::exception& e) {
                    ParseStatus err{
                        line.line,
                        ParseStatus::ERROR,
                        "Invalid NAIF code. Needs to be an integer."
                    };
                    issues.push_back(err);
                    LOG_S(ERROR)
                        << path << ": L" << err.line << ": " << err.message;
                    continue;
                }

                if (!duplicate(include_set, code)) {
                    include_set.emplace_back(
                        NAIFbody{ code }, ParseStatus{ line.line });
                } else {
                    ParseStatus err{ line.line,
                                     ParseStatus::WARNING,
                                     "Duplicate NAIF code." };
                    issues.push_back(err);
                    LOG_S(ERROR)
                        << path << ": L" << err.line << ": " << err.message;
                    continue;
                }
            }
            continue;
        }

        ParseStatus err{ line.line,
                         ParseStatus::ERROR,
                         "Unknown key \"" + line.key + "\"" };
        issues.push_back(err);
        LOG_S(ERROR) << path << ": L" << err.line << ": " << err.message;
    }
}

fs::file_time_type last_write(fs::path path)
{
    bool _ok = fs::exists(path) && !fs::is_directory(path);
    if (_ok) {
        return fs::last_write_time(path);
    } else {
        return fs::file_time_type::min();
    }
}

fs::file_time_type Scenario::current_last_write() const
{
    auto max_t = last_write(path);
    for (const auto& p : ship_files) {
        if (max_t == fs::file_time_type::min()) {
            break;
        }
        max_t = std::max(max_t, last_write(p.value));
    }
    return max_t;
}
}