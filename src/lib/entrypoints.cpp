/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#include <memory>
#include <signal.h>
#include <thread>

#include "entrypoints.hpp"
#include "pdfplotter.hpp"
#include "simulation.hpp"

namespace groho {

volatile sig_atomic_t keep_running = true;

void loop(
    std::string scn_file,
    std::string plot_file,
    std::string sim_folder,
    std::string chart_pdf)
{
    auto simulation = Simulation(scn_file, sim_folder);
    auto plotter    = Plotter(plot_file, sim_folder, chart_pdf);

    signal(SIGINT, [](int) { keep_running = false; });

    while (keep_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    simulation.stop();
    plotter.stop();
}

void sim(std::string scn_file, std::string sim_folder)
{
    auto simulation = Simulation(scn_file, sim_folder);
    simulation.wait_until_done();
}

void chart(std::string plot_file, std::string sim_folder, std::string chart_pdf)
{
    auto plotter = Plotter(plot_file, sim_folder, chart_pdf);
    plotter.wait_until_done();
}

void inspect(std::string kernel_file) {}

}