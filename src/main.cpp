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
    Groho 20.06: A simulator for inter-planetary travel.
    Copyright (c) 2017 - 2020 by Kaushik Ghose. 
    Released under the MIT License. Some rights reserved.
    
)";
}

int main(int argc, char* argv[])
{
    print_license();

    CLI::App app{ "Groho: A simulator for inter-planetary travel" };
    app.require_subcommand(1);

    std::string scn_file, sim_folder, kernel_file;
    bool        non_interactive;

    auto loop = app.add_subcommand(
        "sim",
        "Monitor changes in scenario and plot files,\n"
        "rerun and rechart simulation continuously");
    loop->add_option("simfile", scn_file, "Scenario file")->required();
    loop->add_option("simfolder", sim_folder, "Simulation folder")->required();
    loop->add_flag(
        "--non-interactive",
        non_interactive,
        "Run simulation and exit, instead of looping.");
    loop->callback(
        [&]() { groho::simulate(scn_file, sim_folder, non_interactive); });

    auto programs = app.add_subcommand(
        "programs", "Describe spacecraft programs available");
    programs->callback([&]() { groho::list_programs(); });

    auto inspect = app.add_subcommand("inspect", "Inspect kernel file");
    inspect->add_option("spk", kernel_file, "Kernel file")->required();
    inspect->callback([&]() { groho::inspect(kernel_file); });

    CLI11_PARSE(app, argc, argv);

    return 0;
}