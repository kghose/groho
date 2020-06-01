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
//#include "threadedbuffer.hpp"
#include "simplebuffer.hpp"
#include "v3d.hpp"

namespace fs = std::filesystem;

namespace groho {

class History {

public:
    History(
        double   dt,
        NAIFbody code,
        fs::path path,
        double   rt = 1.00001,
        double   lt = 1e4)
        : code(code)
        , dt(dt)
    {
        sampler = FractalDownsampler(rt, lt);
        buffer
            = std::unique_ptr<SimpleBuffer<V3d>>(new SimpleBuffer<V3d>(path));
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

    FractalDownsampler                 sampler;
    std::shared_ptr<SimpleBuffer<V3d>> buffer;
};

}