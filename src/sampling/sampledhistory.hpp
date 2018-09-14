/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Samples and stores simulation data using the fractal downsampler.
*/

#pragma once

#include <atomic>
#include <fstream>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "body.hpp"
#include "fractaldownsampler.hpp"
#include "vector.hpp"

namespace sim {

/*
    The only tricky thing about SampledHistory is that we often have an
    unsampled point hanging about. This unsampled point only causes problems in
    two cases
        1. Our simulation ends, and we need to force this unsampled point as the
           last point
        2. We want to display the current state of the simulation, and we need
           to include this point in all calculations

    The first version used a std::optional to hold this unsampled point and
    some extra logic to add one to the vector size if we asked for the vector
    while we still had an unsampled point.

    But this led to even more complicated logic when doing searches and
    interpolations.

    Now we'd like to try a simpler method where we always add the unsampled
    point to the end of the array and we keep a switch that tells us if the
    last array point is a (real) sampled point or a (virtual) unsampled one.

    This switch makes most operations pretty simple and is only used
        1. For the internal logic of adding data to the array
        2. By the display array which has some cleverness it uses to only copy
           over the changed points

*/
enum IndexSide { INVALID, LEFT, MIDDLE, RIGHT };

template <typename T> struct SampledHistory {

    SampledHistory() { sampler = FractalDownsampler(1.001, 1e6); }

    bool append(const T& state)
    {
        if (last_point_is_unsampled) {
            data[data.size() - 1] = state;
        } else {
            data.push_back(state);
        }

        if (sampler(state.pos)) {
            last_point_is_unsampled = false;
            return true;
        } else {
            last_point_is_unsampled = true;
            return false;
        }
    }

    // Returns true if we actually flushed
    bool flush()
    {
        if (last_point_is_unsampled) {
            last_point_is_unsampled = false;
            return true;
        } else {
            return false;
        }
    }

    // const std::vector<T>& sampled() const { return data; }
    // std::optional<T>      unsampled() const { return _last_state; }
    // size_t                effective_size() const
    // {
    //     return data.size() + (_last_state ? 1 : 0);
    // }

    // Return smallest index greater than t_s
    std::pair<size_t, IndexSide> index(double t_s) const;

    T at(double t_s) const;

    FractalDownsampler sampler;
    std::vector<T>     data;

    // This is the last point we passed to the sampler that wasn't saved
    // We use this when we flush the buffer or pass it to readers
    bool last_point_is_unsampled = false;
};
}
