/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

A base struct, function signatures and some utility functions for spacecraft
commands.
*/
#pragma once

#include <string>

#include "naifbody.hpp"
#include "parsing.hpp"
#include "state.hpp"
#include "tokens.hpp"
#include "v3d.hpp"

namespace groho {

struct Command {

    Command(const CommandToken& token)
    {
        start = token.start;
        end   = token.start + token.duration;
    }
    virtual ~Command() { ; }

    static std::string usage();

    bool        ready(J2000_s t) { return start < t; }
    bool        expired(J2000_s t) { return end < t; }
    virtual V3d execute(const State&) = 0;

    J2000_s start;
    J2000_s end;
};

void list_all_commands();

class Plan {
public:
    Plan(
        const SpacecraftToken& plan_token, const State& state, size_t self_idx);
    void execute(const State&, V3d& acc);

private:
    std::vector<std::unique_ptr<Command>> commands;

    size_t command_idx = 0;
};

}
