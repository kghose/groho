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

#ifndef NOGUI
#include "app.hpp"
#endif

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
        R"(  Groho 18.08: A simulator for inter-planetary travel and warfare
  Copyright (c) 2017, 2018 by Kaushik Ghose. 
  Released under the MIT License. Some rights reserved)"
              << std::endl;
}

struct Options {
    std::string scenario_file;
    std::string annotation_file;
    bool        interactive = true;
    bool        ok          = false;

    Options(int argc, char* argv[])
    {
        if (argc < 2) {
            print_usage();
            return;
        }

        int pos = 1;
        for (int i = 1; i < argc; i++) {
            std::string opt(argv[i]);

            if (opt[0] == '-') {
                // Optional arguments

                if (opt == "--no-gui") {
                    interactive = false;
                } else {
                    i++; // skip the option argument if we can't understand it
                }

                continue;

            } else {
                // Positional arguments

                switch (pos++) {
                case 1:
                    scenario_file = opt;
                    break;
                case 2:
                    annotation_file = opt;
                    break;
                }
            }
        }
    }

    void print_usage()
    {
        std::cout << "\nUsage:\n"
                  << "groho <scenario file> [annotation file] [--no-gui]"
                  << std::endl;
    }
};

// Periodically monitor scenario file and restart simulator as needed
void simulator_loop(
    sim::Simulator& simulator, Options options, unsigned int interval_ms)
{
    sim::Configuration config = sim::parse_configuration(options.scenario_file);
    simulator.restart_with(config);

    time_t last_mod_time = config.latest_modification();
    while (keep_running) {
        if (last_mod_time < config.latest_modification()) {

            LOG_S(INFO) << "Reloading scenario files";

            config = sim::parse_configuration(options.scenario_file);

            // if (new_scenario != scenario) {
            if (true) {
                LOG_S(INFO) << "Scenario file changed";
                simulator.restart_with(config);
                last_mod_time = config.latest_modification();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }
    simulator.stop();
}

int main(int argc, char* argv[])
{
    print_license();

    loguru::init(argc, argv);

    signal(SIGINT, ctrl_c_pressed);

    Options options(argc, argv);

    sim::Simulator simulator;

    unsigned int interval_ms = 500;

    auto simulator_thread = std::thread(
        simulator_loop, std::ref(simulator), options, interval_ms);

    int ret_val = 0;

#ifndef NOGUI
    if (options.interactive) {
        sim::GrohoApp app({ argc, argv }, simulator);
        ret_val      = app.exec();
        keep_running = false;
    }
#endif

    if (simulator_thread.joinable()) {
        simulator_thread.join();
    }

    return ret_val;
}