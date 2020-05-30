/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Inefficient, single threaded buffer for testing purposes.
*/

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <ctype.h>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
namespace groho {

namespace fs = std::filesystem;

const size_t buf_size = 5000;

template <typename T> class SimpleBuffer {

public:
    SimpleBuffer(fs::path fname)
    {
        file.open(fname, std::ios::binary | std::ios::out);
    }

    void write(const T& k)
    {
        buffer[idx++] = k;
        if (idx == buf_size) {
            file.write((char*)buffer, sizeof(T) * buf_size);
            idx = 0;
        }
    }

    ~SimpleBuffer()
    {
        if (idx > 0) {
            file.write((char*)buffer, sizeof(T) * idx);
        }
    }

private:
    std::ofstream file;
    T             buffer[buf_size];
    size_t        idx = 0;
};
}