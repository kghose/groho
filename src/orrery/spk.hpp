/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

The following code handles loading of NASA/JPL SPK/DAF files that carry
information for planetary ephemerides. The functions defined here use the
chebyshev coefficients stored in the SPK/DAF file to compute vector positions
(x, y, z) for the included bodies
*/

#pragma once

#include <fstream>
#include <memory>
#include <optional>
#include <stdlib.h>
#include <string>
#include <vector>

#include "body.hpp"
#include "v3d.hpp"

namespace groho {

typedef std::vector<double> dbl_vec_t;

struct Elements {
    double    t_mid;  // same as MID
    double    t_half; // same as RADIUS
    dbl_vec_t A;
    size_t    off1, off2, off3;

    void   read(std::ifstream& nasa_spk_file, int n_coeff, int data_type);
    double cheby_eval_one(double t, size_t i0, size_t i1) const;
};

typedef std::vector<Elements> elem_vec_t;

struct Ephemeris {
    int        target_code; // NASA/JPL code for this body
    int        center_code; // NASA/JPL code for reference body
    double     begin_s;     // Start time
    double     interval_s;  // Length of interval
    elem_vec_t elements; // coefficients for just the epoch we are interested in

    void eval(double t, V3d& pos);
};

typedef std::vector<Ephemeris> ephem_vec_t;

// Each summary has a fixed form, reflected here
struct Summary {
    double    begin_second; // initial epoch, as seconds from J2000
    double    end_second;   // final epoch, as seconds from J2000
    u_int32_t target_id;    // target identifier
    u_int32_t center_id;    // center identifier
    u_int32_t frame_id;     // frame identifier (we handle 1 - J2000 - only)
    u_int32_t data_type;    // data type identifier (we handle II or III)
    u_int32_t start_i;      // index (8 byte blocks) where segment data starts
    u_int32_t end_i;        // index (8 byte blocks) where segment data ends
} __attribute__((__packed__));

typedef std::vector<Summary> sumry_vec_t;

struct SpkFile {
    std::string      file_name;
    std::vector<int> bodies;
    std::string      comment;
    sumry_vec_t      summaries;

    static std::optional<SpkFile>
    load(std::string file_name, std::vector<int> bodies);
};

typedef std::vector<SpkFile> spk_vec_t;

}
