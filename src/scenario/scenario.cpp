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

Scenario load_scenario(const std::string& path)
{
    Scenario scenario;
    scenario.path = path;

    for (auto line : InputFile(path).load()) {

        if (line.key == "name") {
            scenario.name = line.value;
            continue;
        }

        if ((line.key == "begin") | (line.key == "end")) {
            auto[date, status] = as_gregorian_date(line.value, line.line);
            if (status.code == ParseStatus::OK) {
                if (line.key == "begin") {
                    scenario.begin = date;
                } else {
                    scenario.end = date;
                }
            } else {
                scenario.issues.push_back(status);
                LOG_S(ERROR)
                    << path << ": L" << status.line << ": " << status.message;
            }
            continue;
        }

        if (line.key == "orrery") {
            if (!duplicate(scenario.orrery_files, line.value)) {
                scenario.orrery_files.emplace_back(
                    line.value, ParseStatus{ line.line });
            } else {
                ParseStatus err{ line.line,
                                 ParseStatus::WARNING,
                                 "Duplicate orrery" };
                scenario.issues.push_back(err);
                LOG_S(ERROR)
                    << path << ": L" << err.line << ": " << err.message;
            }
            continue;
        }

        if (line.key == "ship") {
            if (!duplicate(scenario.ship_files, line.value)) {
                scenario.ship_files.emplace_back(
                    line.value, ParseStatus{ line.line });
            } else {
                ParseStatus err{ line.line,
                                 ParseStatus::WARNING,
                                 "Duplicate flight plan" };
                scenario.issues.push_back(err);
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
                    scenario.issues.push_back(err);
                    LOG_S(ERROR)
                        << path << ": L" << err.line << ": " << err.message;
                    continue;
                }

                if (!duplicate(scenario.include_set, code)) {
                    scenario.include_set.emplace_back(
                        NAIFbody{ code }, ParseStatus{ line.line });
                } else {
                    ParseStatus err{ line.line,
                                     ParseStatus::WARNING,
                                     "Duplicate NAIF code." };
                    scenario.issues.push_back(err);
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
        scenario.issues.push_back(err);
        LOG_S(ERROR) << path << ": L" << err.line << ": " << err.message;
    }
    return scenario;
}
}