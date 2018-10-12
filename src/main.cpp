/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Entry point function for command line program
*/

#include <functional>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <thread>

#include "CLI11.hpp"

#ifndef NOGUI
#include "app.hpp"
#endif

#include "configuration.hpp"
#include "simulator.hpp"

#define LOGURU_IMPLEMENTATION 1
#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

volatile sig_atomic_t keep_running = true;

void ctrl_c_pressed(int) { keep_running = false; }

void print_license()
{
    std::cout << R"(  
    Groho 18.11: A simulator for inter-planetary travel and warfare
    Copyright (c) 2017, 2018 by Kaushik Ghose. 
    Released under the MIT License. Some rights reserved
    
)";
}

struct Options {
    std::string scenario_file;
    std::string annotation_file;
    bool        non_interactive = false;
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

    Options opt;
    bool    show_actions_and_exit;

    CLI::App cli_options{
        "Groho: A simulator for inter-planetary travel and warfare"
    };
    cli_options.add_flag(
        "--no-gui", opt.non_interactive, "Run in non-interactive mode");
    cli_options.add_flag(
        "--actions", show_actions_and_exit, "Show action list");
    cli_options.add_option("scenariofile", opt.scenario_file, "Scenario file")
        ->check(CLI::ExistingFile);
    cli_options
        .add_option("annotationfile", opt.annotation_file, "Annotation file")
        ->check(CLI::ExistingFile);

    try {
        cli_options.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return cli_options.exit(e);
    }

    if (show_actions_and_exit) {
        std::cout << sim::list_available_actions();
        exit(0);
    }

    if (opt.scenario_file == "") {
        std::cout << cli_options.help("", CLI::AppFormatMode::All);
        exit(0);
    }

    loguru::init(argc, argv);

    // This has to come after loguru::init
    signal(SIGINT, ctrl_c_pressed);

    sim::Simulator simulator;

    unsigned int interval_ms = 500;

    auto simulator_thread
        = std::thread(simulator_loop, std::ref(simulator), opt, interval_ms);

    int ret_val = 0;

#ifndef NOGUI
    if (!opt.non_interactive) {
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