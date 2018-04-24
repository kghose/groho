/*
  Code to read NASA SPK files. The aim is to have a complete interface that
  allows us to load a NASA SPK file and then query it for trajectory data
  at arbitrary time points. We can have multiple SPK files loaded at the same
  time: the code gives us a central interface that allows us to query
  trajectories by name without having to keep track of which SPK file the data
  was originally in.

  Order of the SPK files matter if a body is duplicated. In all cases the last
  loaded body will be the one fetched.
*/
#pragma once

#include <fstream>
#include <stdlib.h>
#include <string>

#include "orrery.hpp"

namespace orrery {

namespace daffile {

struct SpkSegment {
    std::string source; // official ephemeris name, like 'DE-0430LE-0430'

    double start_second; // initial epoch, as seconds from J2000
    double end_second; // final epoch, as seconds from J2000
    double start_jd; // start_second, converted to a Julian Date
    double end_jd; // end_second, converted to a Julian Date
    int    center; // integer center identifier
    int    target; // integer target identifier
    size_t center_i; // index of segment/body refered as center
    int    frame; // integer frame identifier
    int    data_type; // integer data type identifier
    size_t start_i; // index where segment starts
    size_t end_i; // index where segment ends

    // Given a julian date fill out (x, y, z) for the passed OrreryBody
    void compute(double jd, OrreryBody& body);
};

typedef std::vector<SpkSegment> SpkSegmentVec;

class SpkOrrery : public Orrery {

    SpkSegmentVec segments;

public:
SpkOrrery() : Orrery() {}
    // Load SPK file "fname" and make the bodies in it available to us
    // Calling this repeatedly will load multiple SPK files.
    // Bodies that appear multiple times will be over written such that
    // only the last read version is kept
    bool load_orrery_model(std::string fname);

    // Fill out the (x, y, z) of each Orrery body and return us an immutable
    // vector containing this information.
    const OrreryBodyVec& get_orrery_at(double jd);
};
}
}
