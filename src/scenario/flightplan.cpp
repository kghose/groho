/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Store bare information from flightplan file.
*/

#include <algorithm>
#include <cctype>
#include <iostream>

#include "flightplan.hpp"
#include "inputfile.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

auto from_line(const J2000_s& t, const Line& line)
{
    PlanElement plan_element;
    ParseError  err;

    auto arguments = split_string(line.value);
    if (arguments.size() > 0) {
        plan_element.t         = t;
        plan_element.command   = arguments[0];
        plan_element.arguments = arguments;
        plan_element.line      = line.line;
        err.error              = false;
    } else {
        err.error   = true;
        err.line    = line.line;
        err.message = "Command missing";
    }

    return std::pair{ plan_element, err };
}

bool compare_plan_t(const PlanElement& p0, const PlanElement& p1)
{
    return p0.t < p1.t;
}

FlightPlan load_flight_plan(const std::string& path)
{
    FlightPlan flight_plan;
    flight_plan.path = path;

    auto flag_error = [&](const ParseError& err) {
        flight_plan.errors.push_back(err);
        LOG_S(ERROR) << path << ": L" << err.line << ": " << err.message;
    };

    for (auto line : InputFile(path).load()) {

        if (line.key == "name") {
            flight_plan.name = line.value;
            continue;
        }

        if (line.key == "initially") {
            auto[plan_element, err] = from_line(0, line);
            if (!err.error) {
                flight_plan.initial_condition = plan_element;
            } else {
                flag_error(err);
            }
            continue;
        }

        if (!std::isdigit(line.key[0])) {
            ParseError err{ true,
                            line.line,
                            "Unknown key \"" + line.key + "\"" };
            flag_error(err);
            continue;
        }

        auto[date, date_err] = as_gregorian_date(line.key, line.line);
        if (date_err.error) {
            flag_error(date_err);
            continue;
        }

        auto[plan_element, err] = from_line(date, line);
        if (!err.error) {
            flight_plan.route.push_back(plan_element);
        } else {
            flag_error(err);
        }
    }

    std::stable_sort(
        flight_plan.route.begin(), flight_plan.route.end(), compare_plan_t);

    return flight_plan;
}
}