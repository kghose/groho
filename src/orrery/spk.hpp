/*
  Code to read NASA SPK files. The aim is to have a minimal interface that
  allows us to load element data from a NASA SPK file and return it as a
  self-contained structure.
*/
#pragma once

#include <fstream>
#include <stdlib.h>
#include <string>

#include "orrery.hpp"

namespace daffile {

struct Emphemeris {
    std::string target_name; // Human readable name for this body
    std::string center_name; // Human readable name for reference body
    int         target_code; // NASA/JPL code for this body
    int         center_code; // NASA/JPL code for reference body
    double      jd_start;    // At what epoch does this emphemeris start
    double      jd_stop;     // At what epoch does this emphemeris end
};

void load_spk(std::ifstream& nasa_spk_file);
}
