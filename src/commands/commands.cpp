/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

A base struct, function signatures and some utility functions for spacecraft
commands.
*/

#include "commands.hpp"
#include "burn.hpp"

namespace groho {

void list_all_commands()
{
    std::cout << "\nAvailable commands:\n\n";
    std::cout << Burn::usage();
}

Plan::Plan(
    const SpacecraftToken& plan_token, const State& state, size_t self_idx)
{
    for (const auto& cmd_token : plan_token.command_tokens) {
        if (cmd_token.command == "burn") {
            commands.emplace_back(new Burn(cmd_token, state, self_idx));
        }
    }
}

void Plan::execute(const State& state, V3d& acc)
{
    if (command_idx == commands.size()) {
        return;
    }

    if (commands[command_idx]->expired(state.t)) {
        command_idx++;
        return;
    }

    if (commands[command_idx]->ready(state.t)) {
        acc += commands[command_idx]->execute(state);
    }
}

}