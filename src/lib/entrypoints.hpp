/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/
#pragma once

#include <string>

namespace groho {

void simulate(std::string scn_file, std::string sim_folder, bool);
void list_programs();
void inspect(std::string kernel_file);

}
