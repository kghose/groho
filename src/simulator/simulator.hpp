/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

This file declares the simulator code
*/

#pragma once

#include <atomic>
#include <thread>

#include "scenario.hpp"

namespace groho {

class Simulator {
public:
    Simulator(std::string scn_file, std::string outdir, bool non_interactive);
    bool scenario_has_changed();
    void quit();
    void wait_until_done() { main_loop_thread.join(); }

private:
    void main_loop();
    void run();

    const std::string scn_file;
    const std::string outdir;
    Scenario          current_scenario;

    std::thread       sim_thread;
    std::thread       main_loop_thread;
    std::atomic<bool> keep_running;
    std::atomic<bool> keep_looping;
};

}
