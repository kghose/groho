/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Subclasses Orrery and incorporates code to read JPL/NASA SPK/DAF files.
One or more SPK files can be loaded into memory and body position computed
at given times.
*/

#include <unordered_set>

#include "solar_system.hpp"
#include "spkorrery.hpp"

namespace orrery {

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
        for (int i = 0; i < un_srt.size(); i++) {
            if (centers.count(un_srt[i]->target_code))
                continue;
            if (centers.count(un_srt[i]->center_code)) {
                srt.push_back(un_srt[i]);
                centers.insert(un_srt[i]->target_code);
            }
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

    for (int i = 0; i < srt.size(); i++) {
        code2index[srt[i]->target_code] = i;
    }

    for (int i = 0; i < srt.size(); i++) {
        if (srt[i]->center_code != 0) {
            ci[i] = code2index[srt[i]->center_code];
        }
    }

    return ci;
}

// For every body in the SPK file create a mirror OrreryBody to store the
// computed position information and some planetary constants. It is important
// to note that this list contains barycenters which are dynamical (virtual)
// points and not for display or gravitational computation.
OrreryBodyVec create_bodies(const EphemerisVec& ephemera)
{
    OrreryBodyVec bodies;
    for (auto e : ephemera) {
        OrreryBody ob;
        if (body_library.count(e->target_code)) {
            ob = body_library[e->target_code];
        }
        bodies.push_back(ob);
    }
    return bodies;
}

bool SpkOrrery::load_orrery_model(std::string fname)
{
    return load_orrery_model(fname, begin_s, end_s);
}

bool SpkOrrery::load_orrery_model(
    std::string fname, double begin_s, double end_s)
{
    std::ifstream nasa_spk_file(fname, std::ios::binary);

    if (!nasa_spk_file) {
        LOG_S(ERROR) << "Could not open " << fname;
        ok = false;
        return false;
    }

    ephemera = sort_ephemerides(
        combine_ephemerides(ephemera, load_spk(nasa_spk_file, begin_s, end_s)));
    center_idx = create_center_indexes(ephemera);
    bodies     = create_bodies(ephemera);

    ok = true;
    return ok;
}

// Fill out the (x, y, z) of each Orrery body and return us an immutable
// vector containing this information.
const OrreryBodyVec& SpkOrrery::get_orrery_at(double s)
{
    for (int i = 0; i < ephemera.size(); i++) {
        set_pos(s, *ephemera[i], bodies[i].state.pos);
        bodies[i].state.t = s;

        if (ephemera[i]->center_code != 0) {
            bodies[i].state.pos += bodies[center_idx[i]].state.pos;
        }
    }
    return bodies;
}

// This is a little expensive because we have to copy over the first set of
// computations
const OrreryBodyVec& SpkOrrery::get_orrery_with_vel_at(double s, double delta_s)
{
    OrreryBodyVec  obv2 = get_orrery_at(s + delta_s);
    OrreryBodyVec& obv1 = const_cast<OrreryBodyVec&>(get_orrery_at(s));
    // Yes, we know what we are doing here ...

    for (size_t i = 0; i < obv1.size(); i++) {
        obv1[i].state.vel = (obv2[i].state.pos - obv1[i].state.pos) / delta_s;
    }

    return obv1;
}
}