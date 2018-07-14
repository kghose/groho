/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Subclasses Orrery and incorporates code to read JPL/NASA SPK/DAF files.
One or more SPK files can be loaded into memory and body position computed
at given times.
*/

#pragma once

#include <cmath>
#include <set>
#include <unordered_map>
#include <vector>

#include "orrery.hpp"
#include "spk.hpp"
#include "vector.hpp"

namespace orrery {

using namespace daffile;

class SpkOrrery : public Orrery {

    EphemerisVec ephemera;

    std::vector<size_t> center_idx;
    // center_idx[i] = index into ephemera for the body that serves as center
    // for i. Undefined for bodies with center 0

public:
    SpkOrrery() { ok = false; }

    SpkOrrery(
        std::set<std::string> fnames, double begin_s = 0, double end_s = 0);

    // Fill out the (x, y, z) of each Orrery body
    void set_orrery_to(double t_s);

    // Fill out the (x, y, z) of each Orrery body and return us an immutable
    // vector containing this information.
    const OrreryBodyVec& get_orrery_at(double t_s);

    // Sometimes we'll need the velocities of the bodies too.
    // For e.g. when we initialize a ship in orbit round a body
    // In this function we invoke get_orrery_at twice with the given interval
    // And then use the position difference to fill out the velocity
    void set_orrery_with_vel_to(double t_s, double delta_s = 10);

    double begin_s;
    double end_s;

private:
    bool load_orrery_model(std::string fname, double begin_s, double end_s);
};
}
