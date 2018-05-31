/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Entry point function for command line program
*/

#include <functional>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <thread>

#include "app.hpp"
#include "buffer.hpp"
#include "scenario.hpp"
#include "simulator.hpp"

#define LOGURU_IMPLEMENTATION 1
#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

volatile sig_atomic_t keep_running = true;

void ctrl_c_pressed(int) { keep_running = false; }

void print_license()
{
    std::cout <<
        R"(  Groho 0.4.0: A simulator for inter-planetary travel and warfare
  Copyright (c) 2017-2018 by Kaushik Ghose. 
  Released under the MIT License. Some rights reserved)"
              << std::endl;
}

void print_usage()
{
    std::cout << "\nUsage:\n"
              << "groho <scenario file> <result file>" << std::endl;
}

// Periodically monitor scenario file and restart simulator as needed
void simulator_loop(
    sim::Simulator& simulator, std::string scn_file, unsigned int interval_ms)
{
    sim::Scenario scenario(scn_file);
    simulator.restart_with(scenario);

    time_t last_mod_time = scenario.latest_modification();
    while (keep_running) {
        if (last_mod_time < scenario.latest_modification()) {

            LOG_S(INFO) << "Reloading scenario files";

            sim::Scenario new_scenario(scenario.fname);
            // if (new_scenario != scenario) {
            if (true) {
                LOG_S(INFO) << "Scenario file changed";
                scenario = new_scenario;
                simulator.restart_with(scenario);
                last_mod_time = scenario.latest_modification();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }
    simulator.stop();
}

int main(int argc, char* argv[])
{
    print_license();

    if (argc < 3) {
        print_usage();
        exit(0);
    }

    loguru::init(argc, argv);

    signal(SIGINT, ctrl_c_pressed);

    std::string scn_file(argv[1]), result_file(argv[2]);

    sim::Simulator simulator;

    unsigned int interval_ms = 500;

    auto simulator_thread = std::thread(
        simulator_loop, std::ref(simulator), scn_file, interval_ms);

    int  ret_val     = 0;
    bool interactive = true;
    if (interactive) {
        sim::GrohoApp app({ argc, argv }, simulator);
        ret_val      = app.exec();
        keep_running = false;
    }

    if (simulator_thread.joinable()) {
        simulator_thread.join();
    }

    return ret_val;
}