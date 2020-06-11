/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

While the spacecraft vel and accel are computed and set explicitly, only the
position of an orrery body is set. We use a rolling buffer to keep track of this
and enable vel and accel computations.
*/
#pragma once

#include "bodyconstant.hpp"
#include "v3d.hpp"

namespace groho {

class DiffableState {
public:
    DiffableState() {}
    DiffableState(
        const std::vector<BodyConstant>& bodies,
        const std::vector<size_t>&       grav_body_idx,
        double                           dt)
        : bodies_(bodies)
        , grav_body_idx_(grav_body_idx)
        , dt(dt)
    {
        for (size_t i = 0; i < bodies_.size(); i++) {
            naif_to_idx_[bodies_[i].code] = i;
        }

        size_t n = bodies_.size();
        vec[0].resize(n);
        vec[1].resize(n);
        vec[2].resize(n);
    }

    /*
       idx    0    1    2
        0    P0   P-2  P-1
        1    P-1  P0   P-2
        2    P-2  P-1  P0

        V0  = (P0 - P-1) / dt
        V-1 = (P-1 - P-2) / dt

        A0  = (V0 - V-1) / dt
    */

    v3d_vec_t& next_pos()
    {
        idx++;
        if (idx > 2) {
            idx = 0;
        }
        return vec[idx];
    }

    const v3d_vec_t& pos() const { return vec[idx]; }

    V3d vel(size_t i) const
    {
        switch (idx) {
        case 0:
            return (vec[0][i] - vec[2][i]) / dt;
        case 1:
            return (vec[1][i] - vec[0][i]) / dt;
        case 2:
            return (vec[2][i] - vec[1][i]) / dt;
        }
    }
    V3d acc(size_t i) const
    {
        switch (idx) {
        case 0:
            //                 V0                            V-1
            return ((vec[0][i] - vec[2][i]) / dt - (vec[2][i] - vec[1][i]) / dt)
                / dt;
        case 1:
            //                 V0                            V-1
            return ((vec[1][i] - vec[0][i]) / dt - (vec[0][i] - vec[2][i]) / dt)
                / dt;
        case 2:
            //                 V0                            V-1
            return ((vec[2][i] - vec[1][i]) / dt - (vec[1][i] - vec[0][i]) / dt)
                / dt;
        }
    }

    size_t idx_of(NAIFbody naif) const { return naif_to_idx_.at(naif); }
    const BodyConstant& body(size_t i) const { return bodies_[i]; }
    const BodyConstant& body(NAIFbody naif) const
    {
        return bodies_[naif_to_idx_.at(naif)];
    }
    const std::vector<size_t>& grav_body_idx() const { return grav_body_idx_; }

private:
    double dt;

    v3d_vec_t vec[3];
    size_t    idx = 0;

    std::vector<BodyConstant>            bodies_;
    std::vector<size_t>                  grav_body_idx_;
    std::unordered_map<NAIFbody, size_t> naif_to_idx_;
};

class ExplicitState {
public:
    ExplicitState() {}
    ExplicitState(std::vector<NAIFbody> codes)
    {
        for (size_t i = 0; i < codes.size(); i++) {
            naif_to_idx_[codes[i]] = i;
        }
        vec[0].resize(codes.size());
        vec[1].resize(codes.size());
        vec[2].resize(codes.size());
    }

    v3d_vec_t&       pos() { return vec[0]; }
    const v3d_vec_t& pos() const { return vec[0]; }
    v3d_vec_t&       vel() { return vec[1]; }
    const v3d_vec_t& vel() const { return vec[1]; }
    v3d_vec_t&       acc() { return vec[2]; }
    const v3d_vec_t& acc() const { return vec[2]; }

    size_t idx_of(NAIFbody naif) const { return naif_to_idx_.at(naif); }

private:
    v3d_vec_t vec[3];

    std::unordered_map<NAIFbody, size_t> naif_to_idx_;
};

struct State {
    State() {}
    State(
        const std::vector<BodyConstant>& bodies,
        const std::vector<size_t>&       grav_body_idx,
        const std::vector<NAIFbody>&     codes,
        double                           dt)
        : orrery(bodies, grav_body_idx, dt)
        , spacecraft(codes)
    {
    }

    DiffableState orrery;
    ExplicitState spacecraft;
};

}