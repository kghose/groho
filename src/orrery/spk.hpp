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

typedef std::vector<Elements> ElementsVec;

struct Ephemeris {
    int         target_code; // NASA/JPL code for this body
    int         center_code; // NASA/JPL code for reference body
    std::string target_name; // Human readable name for this body
    std::string center_name; // Human readable name for reference body
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

EphemerisVec
load_spk(std::ifstream& nasa_spk_file, double begin_jd, double end_jd);
}
