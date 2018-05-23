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

#include <fstream>
#include <string>
#include <vector>

#include "fractaldownsampler.hpp"
#include "vector.hpp"

namespace sim {

struct Metadata {
    int         spkid;
    std::string name;
    uint32_t    color;
    bool        real_body;
};

struct SubBuffer {

    SubBuffer(Metadata meta_)
    {
        meta    = meta_;
        sampler = FractalDownsampler();
    }

    bool append(const Vector& v, bool force)
    {
        if (sampler(v) || force) {
            data.push_back(v);
            return true;
        }
        return false;
    }

    const Metadata& get_metadata() const { return meta; }

    Metadata            meta;
    FractalDownsampler  sampler;
    std::vector<Vector> data;
};

class Buffer {
public:
    size_t          body_count() const { return sub_buffer.size(); }
    const Metadata& metadata(size_t i) const
    {
        return sub_buffer[i].get_metadata();
    }

    void set_simulation_serial(unsigned int _s) { _simulation_serial = _s; }
    unsigned int simulation_serial() const { return _simulation_serial; }
    size_t       point_count() const { return _point_count; }

    // Add another body to the buffer
    void add_body(Metadata meta) { sub_buffer.push_back(SubBuffer(meta)); }

    void lock() const { buffer_mutex.lock(); }
    void release() const { buffer_mutex.unlock(); }

    void append(size_t i, const Vector& v, bool force)
    {
        if (sub_buffer[i].append(v, force))
            _point_count++;
    }

    void finalize() { finalized = true; }

    const std::vector<Vector>& get(size_t i) const
    {
        return sub_buffer[i].data;
    }

private:
    std::atomic<size_t>       _point_count = 0;
    std::atomic<unsigned int> _simulation_serial;

    std::vector<SubBuffer> sub_buffer;
    std::atomic<bool>      finalized = false;
    mutable std::mutex     buffer_mutex;
};
}
