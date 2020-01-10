/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018, 2019, 2020 by Kaushik Ghose. Some rights reserved, see
LICENSE

Tiny utility for inspecting NASA SPK files
*/

#include <iostream>

#define LOGURU_IMPLEMENTATION 1
#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

#include "spk.hpp"
#include "units.hpp"

using namespace groho;

int main(int argc, char* argv[])
{
    std::ifstream nasa_spk_file(argv[1], std::ios::binary);

    daffile::SpkFileSummary summary = daffile::spk_file_summary(nasa_spk_file);

    std::cout << summary.comment << std::endl;

    for (auto ephemeris : summary.ephemera) {
        std::cout << ephemeris.target_code << " -> " << ephemeris.center_code
                  << " t0: " << groho::J2000_s{ ephemeris.begin_s }.as_ut()
                  << " t1: " << groho::J2000_s{ ephemeris.end_s }.as_ut()
                  << std::endl;
    }
}
