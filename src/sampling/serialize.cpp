/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#include <exception>
#include <string>

#include "serialize.hpp"

namespace groho {

Serialize::Serialize(
    const SimParams&             sim_params,
    const std::vector<NAIFbody>& objects,
    const fs::path&              outdir)
{
    if (fs::exists(outdir)) {
        if (!fs::is_directory(outdir)) {
            throw std::runtime_error("Output path must be directory");
        }
    } else {
        fs::create_directories(outdir);
    }

    history.reserve(objects.size());
    for (size_t i = 0; i < objects.size(); i++) {
        auto fname
            = outdir / ("pos" + std::to_string(int(objects[i])) + ".bin");
        history.emplace_back(sim_params, objects[i], fname);
    }
}

void Serialize::append(const v3d_vec_t& pos)
{
    for (size_t i = 0; i < history.size(); i++) {
        history[i].sample(pos[i]);
    }
}

void Serialize::flush(const v3d_vec_t& pos)
{
    for (size_t i = 0; i < history.size(); i++) {
        history[i].flush(pos[i]);
    }
}

}
