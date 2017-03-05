/*
 This implements command line parsing. 
 */
#include "cxxopts.hpp"

struct Options {
    int debug_level;
    std::string simulation_file;
    uint64_t t_max;
    friend std::ostream& operator<<(std::ostream& os, const Options& opt);
};

std::ostream& operator<<(std::ostream& os, const Options& opt)
{
    os << "Debug level: " << opt.debug_level << std::endl
    << "Sim file: " << opt.simulation_file << std::endl
    << "T max: " << opt.t_max << std::endl;
    return os;
}

Options parse_options(int argc, char *argv[]) {
    cxxopts::Options options("groho", "Groho (গ্রহ) is a simulator for inter-planetary travel and warfare.");
    options.add_options()
    ("d,debug", "Debug level", cxxopts::value<int>())
    ("f,sim-file", "Simulation file name", cxxopts::value<std::string>())
    ("t,t-max", "Maximum simulation time", cxxopts::value<uint64_t>())
    ;
    options.parse(argc, argv);
    
    Options opts;
    opts.debug_level = options["d"].as<int>();
    opts.simulation_file = options["f"].as<std::string>();
    opts.t_max = options["t-max"].as<uint64_t>();
    
    return opts;
}
