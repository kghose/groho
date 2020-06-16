/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

This file declares the simulator code
*/

#pragma once

#include <atomic>
#include <thread>

namespace groho {

class Simulator {
public:
    Simulator(std::string scn_file, std::string outdir);
    void stop() { ; }
    void wait_until_done() { sim_thread.join(); }

private:
    void run();

    const std::string scn_file;
    const std::string outdir;

    std::thread sim_thread;
};

}
