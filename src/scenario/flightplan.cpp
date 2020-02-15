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
    ParseStatus status;

    auto arguments = split_string(line.value);
    if (arguments.size() > 0) {
        plan_element.t         = t;
        plan_element.command   = arguments[0];
        plan_element.arguments = arguments;

        status = { line.line, ParseStatus::OK };

    } else {
        status = { line.line, ParseStatus::ERROR, "Command missing" };
    }

    return PlanElementParam{ plan_element, status };
}

bool compare_plan_t(const PlanElementParam& p0, const PlanElementParam& p1)
{
    return p0.value.t < p1.value.t;
}

FlightPlan load_flight_plan(const std::string& path)
{
    FlightPlan flight_plan;
    flight_plan.path = path;

    auto flag_error = [&](const ParseStatus& err) {
        flight_plan.issues.push_back(err);
        LOG_S(ERROR) << path << ": L" << err.line << ": " << err.message;
    };

    for (auto line : InputFile(path).load()) {

        if (line.key == "name") {
            flight_plan.name = line.value;
            continue;
        }

        if (line.key == "initially") {
            auto plan_element = from_line(0, line);
            if (plan_element.status.code == ParseStatus::OK) {
                flight_plan.initial_condition = plan_element;
            } else {
                flag_error(plan_element.status);
            }
            continue;
        }

        if (!std::isdigit(line.key[0])) {
            flag_error({ line.line,
                         ParseStatus::ERROR,
                         "Unknown key \"" + line.key + "\"" });
            continue;
        }

        auto date = as_gregorian_date(line.key, line.line);
        if (date.status.code != ParseStatus::OK) {
            flag_error(date.status);
            continue;
        }

        auto plan_element = from_line(date.value, line);
        if (plan_element.status.code == ParseStatus::OK) {
            flight_plan.route.push_back(plan_element);
        } else {
            flag_error(plan_element.status);
        }
    }

    std::stable_sort(
        flight_plan.route.begin(), flight_plan.route.end(), compare_plan_t);

    return flight_plan;
}
}