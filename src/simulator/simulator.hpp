/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file declares the simulator code
*/

#pragma once

#include <atomic>
#include <thread>

namespace groho {

class Simulator {
public:
    Simulator(std::string scn_file, std::string sim_folder);
    void stop() { ; }
    void wait_until_done() { ; }
};

}
