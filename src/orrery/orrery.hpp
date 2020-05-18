/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Orrery built with SPK files
*/

#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <stdlib.h>
#include <string>
#include <unordered_set>
#include <vector>

#include "spk.hpp"
#include "units.hpp"

namespace groho {

namespace fs = std::filesystem;

struct Kernel {
    std::unordered_set<NAIFbody> codes;
    fs::path                     path;
};

typedef std::vector<Kernel> Kernels;

class Orrery {

public:
    static std::optional<Orrery>
    load(J2000_s begin, J2000_s end, const Kernels& kernels);

    void set_to(J2000_s t, v3d_vec_t& pos);

private:
    struct Body {
        Ephemeris                           ephemeris;
        std::unordered_map<NAIFbody, Body*> children;
        NAIFbody                            parent;
    };

    std::unordered_map<NAIFbody, Body> bodies;
};
}