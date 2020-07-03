/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#include <memory>
#include <signal.h>
#include <thread>

#include "bodyconstant.hpp"
#include "commands.hpp"
#include "entrypoints.hpp"
#include "simulator.hpp"
#include "spk.hpp"
#include "units.hpp"

#define LOGURU_IMPLEMENTATION 1
#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

volatile sig_atomic_t keep_running = true;

void simulate(
    std::string scn_file, std::string sim_folder, bool non_interactive)
{
    auto simulator = Simulator(scn_file, sim_folder, non_interactive);
    if (non_interactive) {
        simulator.wait_until_done();
        return;
    }

    signal(SIGINT, [](int) { keep_running = false; });
    while (keep_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    simulator.quit();
}

void list_commands() { list_all_commands(); }

void inspect(std::string kernel_file)
{
    auto _spk = SpkFile::load(kernel_file);
    if (!_spk) {
        return;
    }

    SpkFile spk = *_spk;
    std::cout << "File comment:\n\n";
    std::cout << spk.comment << std::endl;
    std::cout << "\nBodies and centers:\n\n";
    for (auto [k, summary] : spk.summaries) {
        std::string target = get_body_name(summary.target_id);
        std::string center = get_body_name(summary.center_id);
        std::cout << "    " << target << " -> " << center << "\t"
                  << summary.begin_second.as_ut() << " to "
                  << summary.end_second.as_ut() << std::endl;
    }
}

}