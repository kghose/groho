/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Subclasses Orrery and incorporates code to read JPL/NASA SPK/DAF files.
One or more SPK files can be loaded into memory and body position computed
at given times.
*/

#include <unordered_set>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

#include "solar_system.hpp"
#include "spkorrery.hpp"

namespace sim {

EphemerisVec combine_ephemerides(
    const EphemerisVec& old_ephemera, const EphemerisVec& new_ephemera)
{
    EphemerisVec unsorted_ephemera;

    std::unordered_set<int> centers;

    for (auto e : old_ephemera) {
        unsorted_ephemera.push_back(e);
        centers.insert(e->target_code);
    }

    // Add if not duplicate
    for (auto e : new_ephemera) {
        if (centers.count(e->target_code) == 0) {
            unsorted_ephemera.push_back(e);
            centers.insert(e->target_code);
        } else {
            LOG_S(WARNING) << "Ignoring duplicate body (" << e->target_code
                           << ")";
        }
    }

    return unsorted_ephemera;
}

// Reorder the Ephemerides such that bodies serving as centers come before the
// target bodies.
// This is a multi-pass algorithm. We repeatedly sweep over the original list
// adding bodies to the sorted list as long as we have computed their center
EphemerisVec sort_ephemerides(const EphemerisVec& un_srt)
{
    EphemerisVec srt;

    std::unordered_set<int> centers = { 0 };

    while (srt.size() < un_srt.size()) {
        size_t centers_resolved_this_time = 0;
        for (size_t i = 0; i < un_srt.size(); i++) {
            if (centers.count(un_srt[i]->target_code))
                continue;
            if (centers.count(un_srt[i]->center_code)) {
                srt.push_back(un_srt[i]);
                centers.insert(un_srt[i]->target_code);
                centers_resolved_this_time++;
            }
        }
        if (centers_resolved_this_time == 0) {
            LOG_S(ERROR) << "Orrery has " << un_srt.size() - srt.size()
                         << " bodies with center(s) that do(es) not exist";
            break;
        }
    }

    return srt;
}

// Returns a vector v such that v[i] points to the ephemerides in srt that
// serves as the center for i. Undefined for i whose center is 0
std::vector<size_t> create_center_indexes(const EphemerisVec& srt)
{
    std::unordered_map<int, size_t> code2index;

    std::vector<size_t> ci(srt.size());

    for (size_t i = 0; i < srt.size(); i++) {
        code2index[srt[i]->target_code] = i;
    }

    for (size_t i = 0; i < srt.size(); i++) {
        if (srt[i]->center_code != 0) {
            ci[i] = code2index[srt[i]->center_code];
        }
    }

    return ci;
}

SpkOrrery::SpkOrrery(std::set<std::string> fnames, double begin_s, double end_s)
    : begin_s(begin_s)
    , end_s(end_s)
{
    for (auto& orrery_name : fnames) {
        size_t body_count = ephemera.size();
        load_orrery_model(orrery_name, begin_s, end_s);
        LOG_S(INFO) << ephemera.size() - body_count << " bodies from "
                    << orrery_name;
    }
    LOG_S(INFO) << ephemera.size() << " bodies in Orrery";

    ephemera   = sort_ephemerides(ephemera);
    center_idx = create_center_indexes(ephemera);
    // bodies     = create_bodies(ephemera);
}

bool SpkOrrery::load_orrery_model(
    std::string fname, double begin_s, double end_s)
{
    std::ifstream nasa_spk_file(fname, std::ios::binary);

    if (!nasa_spk_file) {
        LOG_S(ERROR) << "Could not open " << fname;
        return false;
    }

    ephemera = combine_ephemerides(
        ephemera, load_spk(nasa_spk_file, begin_s, end_s));

    return true;
}

// For every body in the SPK file create a mirror OrreryBody to store the
// computed position information and some planetary constants. It is important
// to note that this list contains barycenters which are dynamical (virtual)
// points and not for display or gravitational computation.
std::vector<RockLike::Property> SpkOrrery::get_bodies() const
{
    std::vector<RockLike::Property> bodies;
    for (auto e : ephemera) {
        bodies.push_back(construct_orrery_body(e->target_code));
    }
    return bodies;
}

void set_pos(double s, const Ephemeris& eph, sim::Vector& pos)
{
    const auto& e = eph.evec[std::floor((s - eph.begin_s) / eph.interval_s)];

    pos.x = cheby_eval(e.t_mid, e.t_half, e.X, s);
    pos.y = cheby_eval(e.t_mid, e.t_half, e.Y, s);
    pos.z = cheby_eval(e.t_mid, e.t_half, e.Z, s);
}

// Fill out the (x, y, z) of each Orrery body
void SpkOrrery::set_body_positions(
    double s, std::vector<RockLike::State>& rocks) const
{
    for (size_t i = 0; i < ephemera.size(); i++) {
        set_pos(s, *ephemera[i], rocks[i].pos);
        rocks[i].t_s = s;
    }

    for (size_t i = 0; i < ephemera.size(); i++) {
        if (ephemera[i]->center_code != 0) {
            rocks[i].pos += rocks[center_idx[i]].pos;
        }
    }
}
}