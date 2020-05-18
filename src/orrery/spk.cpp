/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

The following code handles loading of NASA/JPL SPK/DAF files that carry
information for planetary ephemerides.
*/

// TODO: The code as written pays no attention to cache locality. We should do a
// benchmark with a heavyweight kernel (say DE430) where we just compute the
// orrery and then rewrite the code to achieve better cache locality and see how
// much we improve things.

#include <cmath>
#include <fstream>

#include "spklib.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

void Elements::read(std::ifstream& nasa_spk_file, int n_coeff, int data_type)
{
    nasa_spk_file.read((char*)&t_mid, sizeof(double));
    nasa_spk_file.read((char*)&t_half, sizeof(double));

    A.resize(3 * n_coeff);
    nasa_spk_file.read((char*)A.data(), 3 * n_coeff * sizeof(double));
    off1 = n_coeff;
    off2 = 2 * n_coeff;
    off3 = 3 * n_coeff;

    if (data_type == 3) { // Need to skip the unused coefficients
        nasa_spk_file.seekg(3 * n_coeff * 8, std::ios::cur);
    }
}

// https://en.wikipedia.org/wiki/Clenshaw_algorithm#Special_case_for_Chebyshev_series
inline double Elements::cheby_eval_one(double t, size_t i0, size_t i1) const
{
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

inline void Ephemeris::eval(double t, V3d& pos) const
{
    const auto& element = elements[std::floor((t - begin_s) / interval_s)];

    pos.x = element.cheby_eval_one(t, 0, element.off1);
    pos.y = element.cheby_eval_one(t, element.off1, element.off2);
    pos.z = element.cheby_eval_one(t, element.off2, element.off3);
}

bool Summary::valid_time_range(J2000_s begin, J2000_s end) const
{
    if ((begin_second <= begin) && (end_second >= end)) {
        return true;
    } else {
        LOG_S(ERROR) << "Requested range: " << begin.as_ut() << " to "
                     << end.as_ut();
        LOG_S(ERROR) << "Available range: " << begin_second.as_ut() << " to "
                     << end_second.as_ut();
        LOG_S(ERROR) << "Requested time range out of bounds.";
        return false;
    }
}

std::optional<SpkFile> SpkFile::load(const fs::path& path)
{
    std::ifstream nasa_spk_file(path, std::ios::binary);

    auto hdr = read_file_record(nasa_spk_file);
    if (!hdr) {
        LOG_S(ERROR) << path;
        LOG_S(ERROR) << "Not loading because of errors.";
        return {};
    }

    SpkFile spk_file;
    spk_file.path      = path;
    spk_file.comment   = read_comment_blocks(nasa_spk_file, hdr);
    spk_file.summaries = read_summaries(nasa_spk_file, hdr);

    return spk_file;
}

// TODO: Some of the more low level byte reading code could be pushed to spklib?
std::optional<Ephemeris>
SpkFile::load_ephemeris(NAIFbody code, J2000_s begin, J2000_s end) const
{
    std::ifstream nasa_spk_file(path, std::ios::binary);

    auto i = summaries.find(code);
    if (i == summaries.end()) {
        LOG_S(ERROR) << path << ": " << std::to_string(int(code));
        LOG_S(ERROR) << "No such body in file.";
        return {};
    }
    const auto summary = summaries.at(code);

    if (!summary.valid_time_range(begin, end)) {
        LOG_S(ERROR) << path << ": " << std::to_string(int(code));
        return {};
    }

    ElementRecordMetadata erm
        = read_element_record_metadata(nasa_spk_file, summary);

    size_t begin_element = std::floor((begin - erm.init) / erm.intlen);
    size_t end_element   = std::floor((end - erm.init) / erm.intlen);

    size_t n_coeff;
    switch (summary.data_type) {
    case 2:
        n_coeff = (int(erm.rsize) - 2) / 3; // Type II - has one sets of coeffs
        break;
    case 3:
        n_coeff = (int(erm.rsize) - 2) / 6; // Type III - has two sets of coeffs
        break;
    default:
        LOG_S(ERROR) << "Body " << summary.target_id
                     << ": Data is neither Type II or Type III: Skipping";
        return {};
    }

    Ephemeris eph;
    eph.target_code = summary.target_id;
    eph.center_code = summary.center_id;
    eph.begin_s     = erm.init + begin_element * erm.intlen;
    eph.interval_s  = erm.intlen;
    eph.elements.resize(end_element - begin_element + 1);

    size_t internal_offset_byte
        = (begin_element * erm.rsize + summary.start_i - 1) * size_of_double;
    nasa_spk_file.seekg(internal_offset_byte);
    for (size_t i = 0; i < end_element - begin_element + 1; i++) {
        eph.elements[i].read(nasa_spk_file, n_coeff, summary.data_type);
    }

    DLOG_S(INFO) << "Ephemeris for " << summary.target_id << ": " << n_coeff - 1
                 << " order polynomial, " << eph.elements.size()
                 << " elements, ";

    return eph;
}

}