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
    size_t size() { return history.size(); }
    void   append(const v3d_vec_t& pos);
    void   flush(const v3d_vec_t& pos);

private:
    std::vector<History> history;
};

}
