/*
  This handles the downsampling of the data
*/
#pragma once
#include "vector.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

class FractalDownsampler {
public:
    FractalDownsampler(double rt = 1.001, double lt = 1e6)
        : ratio_threshold(rt)
        , linear_threshold(lt)
    {
    }

    bool operator()(const Vector& v)
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

private:
    void accept_sample(const Vector& v)
    {
        cumulative_curve_dist = 0;
        last_sample_v         = v;
        last_v                = v;
    }

    double cumulative_curve_dist = 0;
    Vector last_sample_v         = { 0, 0, 0 };
    Vector last_v                = { 0, 0, 0 };
    double ratio_threshold       = 1.001;
    double linear_threshold      = 1e6;
};

} // namespace sim