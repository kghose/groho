/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Binary search into a vector of state data based on time and interpolate state.
State vector is assumed to be sorted in time ascending manner (as would be
produced by a simulation)
*/

#include "buffer.hpp"

namespace sim {

// For now, just interpolate position
template <typename T>
T interpolate(const T& state0, const T& state1, double t_s)
{
    T state   = state0;
    state.pos = state0.pos
        + (state1.pos - state0.pos)
            * ((t_s - state0.t) / (state1.t - state0.t));
    return state;
}

// Don't do this on an empty vector
template <typename T> T SubBuffer<T>::at(double t_s) const
{
    double t0 = data[0].t, t1 = data[data.size() - 1].t;
    if (t_s < t0) {
        return data[0];
    }
    if (t_s > t1) {
        return data[data.size() - 1];
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
        if (t_s < data[idx1].t) {
            idx2 = idx1;
            idx1 = (idx0 + idx2) / 2;
            continue;
        }
        if (t_s >= data[idx1].t) {
            idx0 = idx1;
            idx1 = (idx0 + idx2) / 2;
            continue;
        }
    }

    return interpolate(data[idx0], data[idx2], t_s);
}

// Simulation::Simulation(
//     std::vector<Rock::Property>& _rocks, std::vector<Ship::Property>& _ships)
// {
//     rocks.resize(_rocks.size());
//     ships.resize(_ships.size());
// }
}
