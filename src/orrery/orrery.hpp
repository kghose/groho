/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Orrery built with SPK files
*/

#pragma once

#include <fstream>
#include <memory>
#include <optional>
#include <stdlib.h>
#include <string>
#include <vector>

#include "spk.hpp"
#include "units.hpp"

namespace groho {

class Orrery {
public:
    static std::optional<Orrery> load(
        std::vector<NAIFbody>    codes,
        std::vector<std::string> file_names,
        J2000_s                  begin,
        J2000_s                  end_s);

    void set_to(J2000_s t, v3d_vec_t& pos);

private:
    ephem_vec_t ephemera;
};
}