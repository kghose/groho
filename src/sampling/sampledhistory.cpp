/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Samples and stores simulation data using the fractal downsampler.
*/

#include "sampledhistory.hpp"

namespace sim {

// For now, just interpolate position
template <typename T>
T interpolate(const T& state0, const T& state1, double t_s)
{
    T state   = state0;
    state.pos = state0.pos
        + (state1.pos - state0.pos)
            * ((t_s - state0.t_s) / (state1.t_s - state0.t_s));
    return state;
}

// TODO: How can we handle enums in a template class
// Return smallest index greater than t_s
// Implemented as binary search
template <typename T>
std::pair<size_t, IndexSide> SampledHistory<T>::index(double t_s) const
{
    if (data.size() == 0) {
        return { 0, IndexSide::INVALID };
    }

    double t0 = data[0].t_s, t1 = data[data.size() - 1].t_s;
    if (t_s <= t0) {
        return { 0, IndexSide::LEFT };
    }
    if (t_s >= t1) {
        return { data.size() - 1, IndexSide::RIGHT };
    }

    // Not as elegant as the recursive formulation, but it gets the job done
    size_t idx0 = 0;
    size_t idx2 = data.size() - 1;
    size_t idx1 = idx2 * ((t_s - t0) / (t1 - t0));
    for (;;) {
        if (idx0 == idx2) {
            break;
        }
        if (idx2 == idx0 + 1) {
            break;
        }
        if (t_s < data[idx1].t_s) {
            idx2 = idx1;
            idx1 = (idx0 + idx2) / 2;
            continue;
        }
        if (t_s >= data[idx1].t_s) {
            idx0 = idx1;
            idx1 = (idx0 + idx2) / 2;
            continue;
        }
    }
    return { idx2, IndexSide::MIDDLE };
}

// This is a slow function that does linear interpolation ...
template <typename T> T SampledHistory<T>::at(double t_s) const
{
    auto [idx, side] = index(t_s);

    switch (side) {

    case IndexSide::INVALID:
        // This should raise an error?
        return T();
        break;

    case IndexSide::LEFT:
    case IndexSide::RIGHT:
        return data[idx];
        break;

    default:
        return interpolate(data[idx - 1], data[idx], t_s);
    }
}

template struct SampledHistory<RockLike::State>;
template struct SampledHistory<ShipLike::State>;
}
