/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Parse scenario input file and return Scenario struct
*/
#include <algorithm>
#include <iostream>

#include "scenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

Scenario::Scenario(Lines& lines)
{
    parse_preamble(lines);
    parse_kernels(lines);
}

void Scenario::parse_preamble(Lines& lines)
{
    for (auto& line : lines) {
        if ((line.key == "start") || (line.key == "end")) {
            auto [date, err] = as_gregorian_date(line.value);
            if (err.length() > 0) {
                line.status.code    = ParseStatus::ERROR;
                line.status.message = err;
            } else {
                if (line.key == "start") {
                    begin = date;
                } else {
                    end = date;
                }
                line.status.code = ParseStatus::OK;
            }
        }
    }
}

void Scenario::parse_kernels(Lines& lines)
{
    bool picking = false;
    for (auto& line : lines) {
        if (line.key == "spk") {
            if (picking) {
                kernels.back().path = line.value;
                picking             = false;
            } else {
                kernels.push_back({ {}, line.value });
            }
            line.status.code = ParseStatus::OK;
        } else if (line.key == "pick") {
            std::unordered_set<NAIFbody> codes;
            for (auto code : split_string(line.value)) {
                try {
                    codes.insert(std::stoi(code));
                } catch (const std::invalid_argument& e) {
                    line.status.code = ParseStatus::ERROR;
                    line.status.message += e.what();
                }
            }
            kernels.push_back({ codes, "" });
            picking = true;
        }
    }
}

}