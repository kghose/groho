/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#pragma once

#include <unordered_map>

#include "naifbody.hpp"
#include "v3d.hpp"

namespace groho {

class CraftState {
public:
    CraftState() {}
    CraftState(std::vector<NAIFbody> codes)
    {
        for (size_t i = 0; i < codes.size(); i++) {
            naif_to_idx_[codes[i]] = i;
        }
        pos.resize(codes.size());
        vel.resize(codes.size());
        acc.resize(codes.size());
    }

    size_t idx_of(NAIFbody naif) const { return naif_to_idx_.at(naif); }

public:
    v3d_vec_t pos, vel, acc;

private:
    std::unordered_map<NAIFbody, size_t> naif_to_idx_;
};

}