/*
  Subclasses Orrery and incorporates code to read JPL/NASA SPK/DAF files.

  Allows us to load one or more SPK files into memory and then does the
  computations required on the loaded data to return helio-centric cartesian
  coordinates at given time points.

  Order of the SPK files matter if a body is duplicated - only data for the last
  load is used.
*/
#pragma once

#include <cmath>
#include <unordered_map>
#include <vector>

#include "orrery.hpp"
#include "spk.hpp"
#include "vector.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace orrery {

using namespace daffile;

inline void set_pos(double s, const Ephemeris& eph, sim::Vector& pos)
{
    Elements e = eph.evec[std::floor((s - eph.begin_s) / eph.interval_s)];

    pos.x = cheby_eval(e.t_mid, e.t_half, e.X, s);
    pos.y = cheby_eval(e.t_mid, e.t_half, e.Y, s);
    pos.z = cheby_eval(e.t_mid, e.t_half, e.Z, s);
}

class SpkOrrery : public Orrery {

    EphemerisVec ephemera;

    std::vector<size_t> center_idx;
    // center_idx[i] = index into ephemera for the body that serves as center
    // for i. Undefined for bodies with center 0

public:
    SpkOrrery()
        : Orrery()
    {
    }
    // Load SPK file "fname" and make the bodies in it available to us
    // Calling this repeatedly will load multiple SPK files.
    // If a body appears more than once, the later entries are disregarded
    bool load_orrery_model(std::string fname, double begin_jd, double end_jd);

    // Fill out the (x, y, z) of each Orrery body and return us an immutable
    // vector containing this information.
    const OrreryBodyVec& get_orrery_at(double jd);
};
}
