/*
 This implements command line parsing. 
 Even cxxopts was too complex for what I needed. So I just used my own, very simple parser.
 
 */

struct Options {
    int debug_level;
    std::string simulation_file;
    uint64_t t_max;
    Options() {
        debug_level = 0;
        t_max = 1000;
    }
    friend std::ostream& operator<<(std::ostream& os, const Options& opt);
};

std::ostream& operator<<(std::ostream& os, const Options& opt)
{
    os << "Debug level: " << opt.debug_level << std::endl
    << "Sim file: " << opt.simulation_file << std::endl
    << "T max: " << opt.t_max << std::endl;
    return os;
}

void print_usage_string(char* argv[]) {
    Options opts;
    std::cout
    << std::endl
    << "Groho (গ্রহ) is a simulator for inter-planetary travel and warfare." << std::endl << std::endl
    << "Usage: " << argv[0]
    << " <SIMFILE> -d <DEBUGLEVEL> -t <TMAX>" << std::endl
    << "Defaults:" << std::endl
    << "d = " << opts.debug_level << std::endl
    << "t = " << opts.t_max << std::endl;
}


Options parse_options(int argc, char *argv[]) {
    try {
        if (argc < 2) throw std::invalid_argument( "Not enough arguments" );
        Options opts;
        opts.simulation_file = argv[1];
        for(int i=2; i < argc - 1; i+=2) {
            std::string s = argv[i];
            if      (s == "-d") opts.debug_level = std::stoi(argv[i + 1]);
            else if (s == "-t") opts.t_max = std::stoi(argv[i + 1]);
            else {std::cout << "Unknown option " << s << std::endl;}
        }
        return opts;
    } catch(...) {
        print_usage_string(argv);
        exit(0);
    }
}
