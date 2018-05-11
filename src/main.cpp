#include <functional>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <thread>

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

std::atomic<bool> scenario_file_changed = true;

void monitor_scenario_file(sim::Scenario& scenario, unsigned int interval_ms)
{
    std::thread([&scenario, interval_ms]() {
        while (keep_running) {
            sim::Scenario new_scenario(scenario.fname);
            if (new_scenario != scenario) {
                scenario              = new_scenario;
                scenario_file_changed = true;
                LOG_S(INFO) << "Scenario file changed";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        }
    })
        .detach();
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

    monitor_scenario_file(scenario, 500);

    while (keep_running) {
        if (scenario_file_changed) {
            simulator.restart_with(scenario);
            scenario_file_changed = false;
        }
        if (simulator.is_running()) {
            simulator.step();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            // sleep for 500ms or so to avoid burning CPU
        }
    }
}