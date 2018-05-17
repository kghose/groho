/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Samples simulation data, stores it and finally serialises it to file
*/

#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "fractaldownsampler.hpp"
#include "vector.hpp"

namespace sim {

struct PositionData {
    PositionData(int spkid_)
    {
        spkid   = spkid_;
        sampler = FractalDownsampler();
    }
    void append(const Vector& v)
    {
        if (sampler(v))
            pos.push_back(v);
    }
    void serialize(std::ofstream& fout)
    {
        LOG_S(INFO) << spkid << ": " << pos.size() << " points";
        unsigned int N = pos.size();
        fout.write((char*)&spkid, sizeof(int));
        fout.write((char*)&N, sizeof(unsigned int));
        fout.write((char*)pos.data(), sizeof(Vector) * pos.size());
        pos.clear();
    }

    int                 spkid;
    std::vector<Vector> pos;
    FractalDownsampler  sampler;
};

struct Buffer {
    Buffer() { ; }
    Buffer(std::string fname_)
    {
        fname = fname_;
        fout.open(fname, std::ios::out | std::ios::binary);
    }

    // Add another body to the buffer
    void add_body(int spkid) { position_data.push_back(PositionData(spkid)); }

    void append(size_t i, const Vector& v) { position_data[i].append(v); }

    void serialize()
    {
        LOG_S(INFO) << "Writing " << position_data.size() << " bodies";
        unsigned int N = position_data.size();
        fout.write((char*)&N, sizeof(unsigned int));
        for (auto& b : position_data) {
            b.serialize(fout);
        }
    }

    std::string   fname;
    std::ofstream fout;

    std::vector<PositionData> position_data;
};
}
