/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

The following code handles loading of NASA/JPL SPK/DAF files that carry
information for planetary ephemerides. The functions defined here use the
chebyshev coefficients stored in the SPK/DAF file to compute vector positions
(x, y, z) for the included bodies
*/

#pragma once

#include <fstream>
#include <memory>
#include <stdlib.h>
#include <string>

#include "orrery.hpp"

namespace daffile {

typedef std::vector<double> dblvec;

struct Elements {
    double t_mid;  // same as MID
    double t_half; // same as RADIUS
    dblvec X;      // coefficients for X
    dblvec Y;      // coefficients for Y
    dblvec Z;      // coefficients for Z
};

// https://en.wikipedia.org/wiki/Clenshaw_algorithm#Special_case_for_Chebyshev_series
inline double cheby_eval(double t_mid, double t_half, const dblvec& A, double t)
{
    double x  = (t - t_mid) / t_half;
    double x2 = 2 * x;
    double Tn, Tn_1 = x, Tn_2 = 1.0;
    double b = A[0] * Tn_2 + A[1] * Tn_1;
    for (size_t i = 2; i < A.size(); i++) {
        Tn = x2 * Tn_1 - Tn_2;
        b += Tn * A[i];
        Tn_2 = Tn_1;
        Tn_1 = Tn;
    }
    return b;
}

typedef std::vector<Elements> ElementsVec;

struct Ephemeris {
    int         target_code; // NASA/JPL code for this body
    int         center_code; // NASA/JPL code for reference body
    double      begin_s;     // Start time
    double      interval_s;  // Length of interval
    ElementsVec evec; // coefficients for just the epoch we are interested in
};

// The consumer of this vector of Ephemerides (SpkOrrery) will want to reorder
// elements of the vector, discard them etc. etc. For example, when we load
// multiple SPK files we want to discard duplicate Ephemerides and reorder the
// set such that bodies used as centers come before their targets.
// An Emphemeris object might be pretty large and it would be nice to not be
// making duplicates of the data during the loading phase. For this reason we
// use pointers
typedef std::shared_ptr<const Ephemeris> EphShrPtr;
typedef std::vector<EphShrPtr>           EphemerisVec;

// In order to conserve RAM we only load elements for the period of time we are
// interested in
EphemerisVec
load_spk(std::ifstream& nasa_spk_file, double begin_s, double end_s);
}
