/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

The following code handles loading of NASA/JPL SPK/DAF files that carry
information for planetary ephemerides. The functions defined here use the
chebyshev coefficients stored in the SPK/DAF file to compute vector positions
(x, y, z) for the included bodies
*/

#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "naifbody.hpp"
#include "units.hpp"
#include "v3d.hpp"

namespace groho {

namespace fs = std::filesystem;

typedef std::vector<double> dbl_vec_t;

struct Elements {
    J2000_s   t_mid;  // same as MID
    J2000_s   t_half; // same as RADIUS
    dbl_vec_t A;
    size_t    off1, off2, off3;

    void   read(std::ifstream& nasa_spk_file, int n_coeff, int data_type);
    double cheby_eval_one(double t, size_t i0, size_t i1) const
    {
        // https://en.wikipedia.org/wiki/Clenshaw_algorithm#Special_case_for_Chebyshev_series
        double x  = (t - t_mid) / t_half;
        double x2 = 2 * x;
        double Tn, Tn_1 = x, Tn_2 = 1.0;
        double b = A[i0] * Tn_2 + A[i0 + 1] * Tn_1;
        for (size_t i = i0 + 2; i < i1; i++) {
            Tn = x2 * Tn_1 - Tn_2;
            b += Tn * A[i];
            Tn_2 = Tn_1;
            Tn_1 = Tn;
        }
        return b;
    }
};

typedef std::vector<Elements> elem_vec_t;

struct Ephemeris {
    int        target_code; // NASA/JPL code for this body
    int        center_code; // NASA/JPL code for reference body
    J2000_s    begin_s;     // Start time
    J2000_s    interval_s;  // Length of interval
    elem_vec_t elements; // coefficients for just the epoch we are interested in

    void eval(double t, V3d& pos) const
    {
        const auto& element = elements[std::floor((t - begin_s) / interval_s)];

        pos.x = element.cheby_eval_one(t, 0, element.off1);
        pos.y = element.cheby_eval_one(t, element.off1, element.off2);
        pos.z = element.cheby_eval_one(t, element.off2, element.off3);
    }
};

typedef std::vector<Ephemeris> ephem_vec_t;

// Each summary has a fixed form, reflected here
struct Summary {
    J2000_s   begin_second; // initial epoch, as seconds from J2000
    J2000_s   end_second;   // final epoch, as seconds from J2000
    u_int32_t target_id;    // target identifier
    u_int32_t center_id;    // center identifier
    u_int32_t frame_id;     // frame identifier (we handle 1 - J2000 - only)
    u_int32_t data_type;    // data type identifier (we handle II or III)
    u_int32_t start_i;      // index (8 byte blocks) where segment data starts
    u_int32_t end_i;        // index (8 byte blocks) where segment data ends

    bool valid_time_range(J2000_s begin, J2000_s end) const;

} __attribute__((__packed__));

typedef std::unordered_map<NAIFbody, Summary> sumry_map_t;

struct SpkFile {
    fs::path    path;
    std::string comment;
    sumry_map_t summaries;

    std::optional<Ephemeris>
    load_ephemeris(NAIFbody code, J2000_s begin, J2000_s end_s) const;

    static std::optional<SpkFile> load(const fs::path& path);
};

typedef std::vector<SpkFile> spk_vec_t;

}
