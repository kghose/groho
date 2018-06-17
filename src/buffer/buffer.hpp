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
#include "groho.hpp"
#include "vector.hpp"

namespace sim {

struct SubBuffer {

    SubBuffer(const Body body_)
    {
        body    = body_;
        sampler = FractalDownsampler(1.001, 1e6);
    }

    bool append(const BodyState& state)
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

    BodyState at(double t_s) const;

    const Body& get_metadata() const { return body; }

    Body                     body;
    FractalDownsampler       sampler;
    std::vector<BodyState>   data;
    std::optional<BodyState> _last_state;
};

class Buffer {
public:
    size_t      body_count() const { return sub_buffer.size(); }
    const Body& metadata(size_t i) const
    {
        return sub_buffer[i].get_metadata();
    }

    void set_simulation_serial(unsigned int _s) { _simulation_serial = _s; }
    unsigned int simulation_serial() const { return _simulation_serial; }
    size_t       point_count() const { return _point_count; }

    // Add another body to the buffer
    void add_body(const Body body)
    {
        sub_buffer.push_back(SubBuffer(body));
        id_to_index[body.code] = sub_buffer.size() - 1;
    }

    void lock() const { buffer_mutex.lock(); }
    void release() const { buffer_mutex.unlock(); }

    void append(size_t i, const BodyState& state)
    {
        if (sub_buffer[i].append(state))
            _point_count++;
    }

    bool flush()
    {
        std::lock_guard<std::mutex> lock(buffer_mutex);

        bool flushed = false;
        for (auto& b : sub_buffer) {
            if (b.flush()) {
                flushed = true;
                _point_count++;
            }
        }
        return flushed;
    }

    std::optional<size_t> get_index(int spkid) const
    {
        auto result = id_to_index.find(spkid);
        if (result != id_to_index.end()) {
            return result->second;
        } else {
            return {};
        }
    }

    const std::vector<BodyState>& get(size_t i) const
    {
        return sub_buffer[i].data;
    }

    BodyState at(size_t i, double t_s) const;

private:
    std::atomic<size_t>       _point_count = 0;
    std::atomic<unsigned int> _simulation_serial;

    std::unordered_map<int, size_t> id_to_index;

    std::vector<SubBuffer> sub_buffer;
    mutable std::mutex     buffer_mutex;
};
}
