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
    parse_plans(lines);
    sort_plans();
    log_issues(lines);
}

void Scenario::parse_preamble(Lines& lines)
{
    for (auto& line : lines) {
        if ((line.key == "start") || (line.key == "end")) {
            auto [date, err] = as_gregorian_date(line.value);
            if (err.length() > 0) {
                line.status.code    = ParseStatus::ERROR;
                line.status.message = err;
                continue;
            }

            if (line.key == "start") {
                begin = date;
            } else {
                end = date;
            }
            line.status.code = ParseStatus::OK;
        }
    }
}

void Scenario::parse_kernels(Lines& lines)
{
    bool picking = false;
    for (auto& line : lines) {
        if (line.status.code != ParseStatus::PENDING) {
            continue;
        }

        if (line.key == "spk") {
            if (picking) {
                kernel_tokens.back().path = line.value;
                picking                   = false;
            } else {
                kernel_tokens.push_back({ {}, line.value });
            }
            line.status.code = ParseStatus::OK;

        } else if (line.key == "pick") {
            std::unordered_set<NAIFbody> codes;
            line.status.code = ParseStatus::OK;
            for (auto code : split_string(line.value)) {
                try {
                    codes.insert(std::stoi(code));
                } catch (const std::invalid_argument& e) {
                    line.status.code = ParseStatus::ERROR;
                    line.status.message += e.what();
                }
            }
            kernel_tokens.push_back({ codes, "" });
            picking = true;
        }
    }
}

bool probably_a_date(std::string s) { return s.find(":") != std::string::npos; }

void Scenario::parse_plans(Lines& lines)
{
    auto no_associated_craft = [](Line& line) {
        line.status.code    = ParseStatus::ERROR;
        line.status.message = "No craft associated with plan line.";
    };

    std::string plan_name;
    for (auto& line : lines) {
        if (line.status.code != ParseStatus::PENDING) {
            continue;
        }

        if (line.key == "plan") {
            plan_name                               = line.value;
            spacecraft_tokens[plan_name].craft_name = plan_name;
            line.status.code                        = ParseStatus::OK;

        } else if (line.key == "orbiting") {
            if (plan_name == "") {
                no_associated_craft(line);
                continue;
            }

            auto tokens = split_string(line.value);
            if (tokens.size() != 2) {
                line.status
                    = { ParseStatus::ERROR, "Expecting two parameters" };
                continue;
            }

            spacecraft_tokens[plan_name].initial_condition
                = { 0, 0, "orbiting", tokens };
            line.status.code = ParseStatus::OK;

        } else if (probably_a_date(line.key)) {
            if (plan_name == "") {
                no_associated_craft(line);
                continue;
            }

            auto [date, err] = as_gregorian_date(line.key);
            if (err.length() > 0) {
                line.status.code    = ParseStatus::ERROR;
                line.status.message = err;
                continue;
            }

            auto tokens = split_string(line.value);
            if (tokens.size() < 2) {
                line.status = { ParseStatus::ERROR,
                                "Expecting at least a duration and a command" };
                continue;
            }

            double duration;
            try {
                duration = std::stod(tokens[0]);
            } catch (const std::exception& e) {
                line.status = { ParseStatus::ERROR,
                                "Couldn't parse command duration "
                                    + std::string(e.what()) };
                continue;
            }

            spacecraft_tokens[plan_name].command_tokens.push_back(
                { date,
                  duration,
                  tokens[1],
                  std::vector<std::string>(tokens.begin() + 2, tokens.end()) });
            line.status.code = ParseStatus::OK;
        }
    }
}

void Scenario::sort_plans()
{
    for (auto& [_, craft_tok] : spacecraft_tokens) {
        std::sort(
            craft_tok.command_tokens.begin(),
            craft_tok.command_tokens.end(),
            [](const CommandToken& cmd_tok1, const CommandToken& cmd_tok2) {
                return cmd_tok1.start < cmd_tok2.start;
            });
    }
}

void Scenario::log_issues(const Lines& lines) const
{
    for (auto& line : lines) {
        if (line.status.code != ParseStatus::OK) {
            auto message = line.status.code == ParseStatus::PENDING
                ? "I don't understand this line"
                : line.status.message;

            LOG_S(ERROR) << line.file_name << ":" << line.line << " "
                         << message;
        }
    }
}

}