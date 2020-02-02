/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Parse scenario input file and return Scenario struct
*/
#include <iostream>

#include "inputfile.hpp"
#include "scenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

Scenario load_scenario(const std::string& path)
{
    Scenario scenario;
    for (auto line : InputFile(path).load()) {

        if (line.key == "name") {
            scenario.name = line.value;
            continue;
        }

        if (line.key == "begin" | line.key == "end") {
            auto [date, err] = as_gregorian_date(line.value, line.line);
            if (!err.error) {
                if (line.key == "begin") {
                    scenario.begin = date;
                } else {
                    scenario.end = date;
                }
            } else {
                scenario.errors.push_back(err);
                LOG_S(ERROR)
                    << path << ": L" << err.line << ": " << err.message;
            }
            continue;
        }

        if (line.key == "orrery") {
            scenario.orrery_files.insert(line.value);
            continue;
        }

        if (line.key == "ship") {
            scenario.ship_files.insert(line.value);
            continue;
        }

        if (line.key == "use-only") {
            for (auto id : split_string(line.value)) {
                try {
                    scenario.include_set.insert(NAIFbody(std::stoi(id)));
                } catch (std::exception& e) {
                    ParseError err{
                        true,
                        line.line,
                        "Invalid NAIF code. Needs to be an integer."
                    };
                    scenario.errors.push_back(err);
                    LOG_S(ERROR)
                        << path << ": L" << err.line << ": " << err.message;
                }
            }
            continue;
        }

        ParseError err{ true, line.line, "Unknown key \"" + line.key + "\"" };
        scenario.errors.push_back(err);
        LOG_S(ERROR) << path << ": L" << err.line << ": " << err.message;
    }
    return scenario;
}
}