/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Subclasses Orrery and incorporates code to read JPL/NASA SPK/DAF files.
One or more SPK files can be loaded into memory and body position computed
at given times.
*/

#pragma once

#include <cmath>
#include <set>
#include <unordered_map>
#include <vector>

#include "body.hpp"
#include "spk.hpp"
//#include "vector.hpp"

namespace sim {

using namespace daffile;

class SpkOrrery {

public:
    SpkOrrery() = default;

    SpkOrrery(
        std::set<std::string> fnames, double begin_s = 0, double end_s = 0);

    // Fill out a catalog of bodies with indeterminate state
    std::vector<RockLike::Property> get_bodies() const;

    // Fill out the (x, y, z) of each Orrery body
    void set_body_positions(double t_s, std::vector<RockLike::State>&) const;

    double begin_s;
    double end_s;

private:
    bool load_orrery_model(std::string fname, double begin_s, double end_s);

    EphemerisVec ephemera;

    std::vector<size_t> center_idx;
    // center_idx[i] = index into ephemera for the body that serves as center
    // for i. Undefined for bodies with center 0
};
}
