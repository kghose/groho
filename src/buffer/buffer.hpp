/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Samples and stores simulation data.

This supplies a simple interface to the Simulator for saving simulation data
and to other entities - like a display - for reading data. Reading and writing
are thread-safe.

In this first iteration each sub-buffer is locked for writing and reading. It
sounds like this will be a lot of overhead, but it is easy to code and we
should see if this is fast enough for us.
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

// template <typename T> struct data_t {
//     const std::vector<BodyState>&   sampled;
//     const std::optional<BodyState>& unsampled;

//     size_t required_size()
//     {
//         size_t size = sampled.size();
//         if (unsampled) {
//             size += 1;
//         }
//         return size;
//     }
// };

template <typename T> struct SubBuffer {

    SubBuffer() { sampler = FractalDownsampler(1.001, 1e6); }

    bool append(const T& state)
    {
        if (sampler(state.pos)) {
            data.push_back(state);
            _last_state = {};
            return true;
        } else {
            _last_state = state;
            return false;
        }
    }

    // Returns true if we actually flushed
    bool flush()
    {
        if (_last_state) {
            data.push_back(*_last_state);
            _last_state = {};
            return true;
        } else {
            return false;
        }
    }

    const std::vector<T>& sampled() { return data; }
    std::optional<T>      unsampled() { return _last_state; }
    size_t effective_size() { return data.size() + (_last_state ? 1 : 0); }

    T at(double t_s) const;

    FractalDownsampler sampler;
    std::vector<T>     data;

    // This is the last point we passed to the sampler that wasn't saved
    // We use this when we flush the buffer or pass it to readers
    std::optional<T> _last_state;
};

// struct SimulationSegment {
//     NAIFbody naif;
//     double   start_ts;
//     double   end_ts;
// };

// template <typename T> struct Record {
//     typename T::Property         property;
//     SubBuffer<typename T::State> history;
// };

// struct Simulation : public Objects<Record> {

//     //
//     void append(const State&);

//     // Add any unsampled data into the history
//     bool flush();

//     // We'll be writing to this and people will be clamoring to read from
//     this
//     // and we must have order!
//     mutable std::mutex buffer_mutex;

//     // This allows a reader to figure out if the simulation has been
//     restarted
//     // since their last read
//     std::atomic<size_t> simulation_serial = 0;

//     // This allows a reader to figure out if the data has changed since their
//     // last read
//     std::atomic<size_t> point_count = 0;
// };
}
