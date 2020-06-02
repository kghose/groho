/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

During the simulation, the simulator passes position data at each time step to
the Serializer class, which takes care of downsampling and saving the data. The
serializer also supplies velocity and acceleration data on request.
*/

#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "history.hpp"
#include "naifbody.hpp"
#include "simparams.hpp"

namespace groho {

namespace fs = std::filesystem;

class Serialize {

public:
    Serialize() { ; }
    Serialize(
        const SimParams&             sim_params,
        const std::vector<NAIFbody>& objects,
        const fs::path&              outdir);
    void      append(const v3d_vec_t& pos);
    const V3d pos(size_t idx) const { return history[idx].pos(); }
    const V3d vel(size_t idx) const { return history[idx].vel(); }
    const V3d acc(size_t idx) const { return history[idx].acc(); }

private:
    std::vector<History> history;
};

}
