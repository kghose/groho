#include <iostream>
#include <signal.h>

#include "scenario.hpp"
#include "simulator.hpp"

#define LOGURU_IMPLEMENTATION 1
#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

volatile sig_atomic_t keep_running = true;

void ctrl_c_pressed(int) { keep_running = false; }

void print_usage()
{
    std::cout << "\nUsage:\n"
              << "groho <scenario file> <result file>" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        print_usage();
        exit(0);
    }

    loguru::init(argc, argv);

    signal(SIGINT, ctrl_c_pressed);

    std::string    scenario_file(argv[1]), result_file(argv[2]);
    sim::Scenario  scenario(scenario_file);
    sim::Simulator simulator(result_file);

    while (keep_running) {
        if (scenario.is_changed()) {
            simulator.restart_with(scenario);
        }
        simulator.step();
    }
}