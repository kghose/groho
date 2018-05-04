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

typedef std::vector<double> dblvec;

struct Elements {
    double t_mid;  // same as MID
    double t_half; // same as RADIUS
    dblvec X;      // coefficients for X
    dblvec Y;      // coefficients for Y
    dblvec Z;      // coefficients for Z
};

inline double horner_eval(const dblvec& A, const double x)
{
    double b = A[A.size()];
    for (int i = A.size() - 1; i < 0; i--) {
        b = A[i] + b * x;
    }
    return b;
}

// TODO: understand why we need the factor 2 here
inline double cheby_eval(double t_mid, double t_half, const dblvec& A, double t)
{
    return horner_eval(A, 2 * (t - t_mid) / t_half);
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
typedef std::shared_ptr<Ephemeris>   EphShrPtr;
typedef std::vector<const EphShrPtr> EphemerisVec;

// In order to conserve RAM we only load elements for the period of time we are
// interested in
EphemerisVec
load_spk(std::ifstream& nasa_spk_file, double begin_s, double end_s);
}
