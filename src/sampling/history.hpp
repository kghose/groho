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
// #include "threadedbuffer.hpp"
#include "simplebuffer.hpp"
#include "v3d.hpp"

namespace fs = std::filesystem;

namespace groho {

class History {

public:
    History(const SimParams& sim_params, NAIFbody code, fs::path path)
        : dt(sim_params.dt)
        , code(code)
        , rotx(-3.14159265358979323846264338327950288419 * 23.5 / 180.0)
    {
        sampler = FractalDownsampler(sim_params.rt, sim_params.lt);
        // buffer.reset(new ThreadedBuffer<V3d>(path));
        buffer.reset(new SimpleBuffer<V3d>(path));
    }

    void sample(const V3d& pos)
    {
        if (sampler(pos)) {
            // buffer->write(rotx(pos));
            buffer->write(pos);
        }
    }

    ~History()
    {
        V3d last_pos;
        if (sampler.flush(last_pos)) {
            // buffer->write(rotx(last_pos));
            buffer->write(last_pos);
        }
    }

private:
    const double   dt;
    const NAIFbody code;

    FractalDownsampler sampler;
    RotateX            rotx;

    // std::shared_ptr<ThreadedBuffer<V3d>> buffer;
    std::shared_ptr<SimpleBuffer<V3d>> buffer;
};

}