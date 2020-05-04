/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/
#pragma once

#include <string>

namespace groho {

class Simulation {
public:
    Simulation(std::string scn_file, std::string sim_folder) { ; }
    void stop() { ; }
    void wait_until_done() { ; }
};

}