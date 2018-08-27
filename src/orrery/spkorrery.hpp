/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Orrery. Glues together code to read JPL/NASA SPK/DAF files and compute body
positions based on that data.
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

namespace sim {

using namespace daffile;

class SpkOrrery {

public:
    SpkOrrery() = default;

    SpkOrrery(
        std::set<std::string> fnames, double begin_s = 0, double end_s = 0);

    // Fill out a catalog of bodies
    std::vector<RockLike::Property> get_bodies() const;

    // Fill out the (x, y, z) of each Orrery body
    void set_body_positions(double t_s, Collection<SnapShotV<RockLike>>&) const;

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
