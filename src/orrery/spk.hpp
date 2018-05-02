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

void load_spk(std::ifstream& nasa_spk_file);
}
