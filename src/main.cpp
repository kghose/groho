/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see
LICENSE

Entry point function for command line program
*/

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

#include <signal.h>
#include <stdlib.h>

#include "CLI11.hpp"

#include "simulator.hpp"

#define LOGURU_IMPLEMENTATION 1
#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

void print_license()
{
    std::cout << R"(  
    Groho 20.02: A simulator for inter-planetary travel and warfare
    Copyright (c) 2017 - 2020 by Kaushik Ghose. 
    Released under the MIT License. Some rights reserved.
    
)";
}

struct Options {
    std::string scenario_file;
    std::string output_folder;
};

Options parse_arguments(int argc, char* argv[])
{
    bool        show_actions_and_exit;
    std::string scenario_file, output_folder;

    CLI::App cli_options{ "Groho: A simulator for inter-planetary travel" };
    cli_options.add_flag(
        "--actions", show_actions_and_exit, "Show action list");
    cli_options.add_option("scenariofile", scenario_file, "Scenario file");
    cli_options.add_option("outputfolder", output_folder, "Output folder");

    try {
        cli_options.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        exit(cli_options.exit(e));
    }

    if (show_actions_and_exit) {
        std::cout << groho::Simulator::list_available_actions();
        exit(0);
    }

    if (scenario_file == "" | output_folder == "") {
        std::cout << cli_options.help("", CLI::AppFormatMode::All);
        exit(0);
    }

    return Options{ scenario_file, output_folder };
}

volatile sig_atomic_t keep_running = true;

void ctrl_c_pressed(int) { keep_running = false; }

int main(int argc, char* argv[])
{
    print_license();

    Options opt = parse_arguments(argc, argv);

    loguru::init(argc, argv);

    // This has to come after loguru::init
    signal(SIGINT, ctrl_c_pressed);

    using namespace std::chrono_literals;
    auto poll_interval_ms = 500ms;

    groho::Simulator simulator(opt.scenario_file, opt.output_folder);

    while (keep_running && simulator.ok()) {
        if (simulator.scenario_changed()) {
            simulator.restart();
        }
        std::this_thread::sleep_for(poll_interval_ms);
    }

    simulator.exit();

    return 0;
}