/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include "fractaldownsampler.hpp"
#include "naifbody.hpp"
#include "simparams.hpp"
#include "threadedbuffer.hpp"
// #include "simplebuffer.hpp"
#include "v3d.hpp"

namespace fs = std::filesystem;

namespace groho {

class History {

public:
    History(const SimParams& sim_params, NAIFbody code, fs::path path)
        : dt(sim_params.dt)
        , code(code)
    {
        sampler = FractalDownsampler(sim_params.rt, sim_params.lt);
        buffer.reset(new ThreadedBuffer<V3d>(path));
        // buffer.reset(new SimpleBuffer<V3d>(path));
    }

    void sample(const V3d& pos)
    {
        if (sampler(pos)) {
            buffer->write(pos);
        }
    }

    void flush(const V3d& pos) { buffer->write(pos); }

private:
    const double   dt;
    const NAIFbody code;

    FractalDownsampler sampler;

    std::shared_ptr<ThreadedBuffer<V3d>> buffer;
    // std::shared_ptr<SimpleBuffer<V3d>> buffer;
};

}