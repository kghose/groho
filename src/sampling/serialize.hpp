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

namespace fs = std::filesystem;

namespace groho {

class Serialize {

public:
    Serialize(double dt, const std::vector<NAIFbody>& objects, fs::path path);
    void       append(const v3d_vec_t& pos);
    const V3d& pos(size_t idx) const;
    const V3d& vel(size_t idx) const;
    const V3d& acc(size_t idx) const;

private:
    std::vector<History> history;
};

}
