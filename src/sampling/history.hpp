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
#include "threadedbuffer.hpp"
#include "v3d.hpp"

namespace fs = std::filesystem;

namespace groho {

class History {

public:
    History(double dt, NAIFbody code, fs::path path)
        : dt(dt)
        , code(code)
    {
        buffer = std::unique_ptr<ThreadedBuffer<V3d>>(
            new ThreadedBuffer<V3d>(path));
    }
    ~History();

    void sample(const V3d& pos);

    V3d pos() const;
    V3d vel() const;
    V3d acc() const;

private:
    NAIFbody code;

    double dt;
    V3d    _pos[3];
    size_t _idx = 0;

    FractalDownsampler                   sampler;
    std::unique_ptr<ThreadedBuffer<V3d>> buffer;
};

}