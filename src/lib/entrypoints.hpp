/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/
#pragma once

#include <string>

namespace groho {

void loop(
    std::string scn_file,
    std::string plot_file,
    std::string sim_folder,
    std::string chart_pdf);

void sim(std::string scn_file, std::string sim_folder);

void chart(
    std::string plot_file, std::string sim_folder, std::string chart_pdf);

void inspect(std::string kernel_file);

}
