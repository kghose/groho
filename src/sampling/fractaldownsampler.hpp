/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This handles the downsampling of the data
*/

#pragma once

#include <limits>

#include "v3d.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

class FractalDownsampler {
public:
    FractalDownsampler(double rt = 1.001, double lt = 1e6)
        : ratio_threshold(rt)
        , linear_threshold(lt)
    {
        cumulative_curve_dist = std::numeric_limits<double>::infinity();
    }

    bool operator()(const V3d& v)
    {
        cumulative_curve_dist += (v - last_v).norm();
        double linear_dist = (v - last_sample_v).norm();

        if (((cumulative_curve_dist / linear_dist) > ratio_threshold)
            | (abs(cumulative_curve_dist - linear_dist) > linear_threshold)) {
            accept_sample(v);
            return true;
        }
        last_v = v;
        return false;
    }

    bool flush(V3d& v)
    {
        if (cumulative_curve_dist != 0) {
            v = last_v;
            return true;
        } else {
            return false;
        }
    }

private:
    void accept_sample(const V3d& v)
    {
        cumulative_curve_dist = 0;
        last_sample_v         = v;
        last_v                = v;
    }

    double cumulative_curve_dist = 0;
    V3d    last_sample_v         = { 0, 0, 0 };
    V3d    last_v                = { 0, 0, 0 };
    double ratio_threshold       = 1.001;
    double linear_threshold      = 1e6;
};

} // namespace sim