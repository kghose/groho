/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see
LICENSE

Entry point function for command line program
*/

#include "CLI11.hpp"

#include "entrypoints.hpp"

void print_license()
{
    std::cout << R"(  
    Groho 20.05: A simulator for inter-planetary travel and warfare
    Copyright (c) 2017 - 2020 by Kaushik Ghose. 
    Released under the MIT License. Some rights reserved.
    
)";
}

int main(int argc, char* argv[])
{
    print_license();

    CLI::App app{ "Groho: A simulator for inter-planetary travel" };
    app.require_subcommand(1);

    std::string scn_file, plot_file, sim_folder, chart_pdf;

    auto loop = app.add_subcommand(
        "loop",
        "Monitor changes in scenario and plot files,\n"
        "rerun and rechart simulation continuously");
    loop->add_option("simfile", scn_file, "Scenario file");
    loop->add_option("plotfile", plot_file, "Plot file");
    loop->add_option("simfolder", sim_folder, "Simulation folder");
    loop->add_option("chart", chart_pdf, "Chart PDF");
    loop->callback(
        [&]() { groho::loop(scn_file, plot_file, sim_folder, chart_pdf); });

    auto sim = app.add_subcommand("sim", "Simulate scenario and exit");
    sim->add_option("simfile", scn_file, "Scenario file");
    sim->add_option("simfolder", sim_folder, "Simulation folder");
    sim->callback([&]() { groho::sim(scn_file, sim_folder); });

    auto chart = app.add_subcommand("chart", "Chart simulation and exit");
    chart->add_option("plotfile", plot_file, "Plot file");
    chart->add_option("simfolder", sim_folder, "Simulation folder");
    chart->add_option("chart", chart_pdf, "Chart PDF");
    chart->callback([&]() { groho::chart(plot_file, sim_folder, chart_pdf); });

    CLI11_PARSE(app, argc, argv);

    return 0;
}