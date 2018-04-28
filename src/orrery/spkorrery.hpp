/*
  Subclasses Orrery and incorporates code to read JPL/NASA SPK/DAF files.

  Allows us to load one or more SPK files into memory and then does the
  computations required on the loaded data to return helio-centric cartesian
  coordinates at given time points.

  Order of the SPK files matter if a body is duplicated - only data for the last
  load is used.
*/
#pragma once

#include "orrery.hpp"
#include "spk.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace orrery {

using namespace daffile;

struct SpkSegment {
    std::string source; // official ephemeris name, like 'DE-0430LE-0430'

    double start_second; // initial epoch, as seconds from J2000
    double end_second;   // final epoch, as seconds from J2000
    double start_jd;     // start_second, converted to a Julian Date
    double end_jd;       // end_second, converted to a Julian Date
    int    center;       // integer center identifier
    int    target;       // integer target identifier
    size_t center_i;     // index of segment/body refered as center
    int    frame;        // integer frame identifier
    int    data_type;    // integer data type identifier
    size_t start_i;      // index where segment starts
    size_t end_i;        // index where segment ends

    // Given a julian date fill out (x, y, z) for the passed OrreryBody
    void compute(double jd, OrreryBody& body);
};

typedef std::vector<SpkSegment> SpkSegmentVec;

class SpkOrrery : public Orrery {

    SpkSegmentVec segments;

public:
    SpkOrrery()
        : Orrery()
    {
    }
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
