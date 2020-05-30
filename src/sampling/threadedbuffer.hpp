/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Double buffer class for writing out to disk.

See the double buffer activity diagram (docs/Readme.md). Also see my blog post:
https://kaushikghose.wordpress.com/2020/03/20/buffering-writes/)

Example usage:

int main(int argc, char* argv[])
{
    ThreadedBuffer<size_t> buffer("test.bin");
    for (size_t j = 0; j < (1 << 27); j++) {
        size_t k = (size_t)(j * 1.5 - j * 1.1 + j / 3.2);
        buffer.write(k);
    }
}*/

#include <atomic>
#include <chrono>
#include <ctype.h>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>

namespace groho {

namespace fs = std::filesystem;

const size_t buf_size = 5000;

template <typename T> class ThreadedBuffer {
    enum BufferId { one = 0, two = 1, neither = 3 };

public:
    ThreadedBuffer(fs::path fname)
    {
        buffer[0] = new T[buf_size];
        buffer[1] = new T[buf_size];
        idx[0] = idx[1] = 0;

        file.open(fname, std::ios::binary | std::ios::out);

        write_b = 0;
        m[write_b].lock();

        saver = std::thread(&ThreadedBuffer::save_buffer_to_disk, this);
    }

    ~ThreadedBuffer()
    {
        release_write_buffer();
        saver.join();
        delete[] buffer[0];
        delete[] buffer[1];
    }

    void write(const T& k)
    {
        buffer[write_b][idx[write_b]] = k;
        idx[write_b]++;

        if (idx[write_b] == buf_size) {
            release_write_buffer();
        }
    }

private:
    std::ofstream file;
    T*            buffer[2];
    size_t        idx[2];
    size_t        write_b = 0;
    std::mutex    m[2];

    std::thread saver;

    void save_buffer_to_disk()
    {
        size_t save_b = 0;
        for (;;) {
            const std::lock_guard<std::mutex> save_guard(m[save_b]);
            file.write((char*)buffer[save_b], sizeof(T) * idx[save_b]);
            if (idx[save_b] < buf_size) {
                break;
            }
            save_b = 1 - save_b;
        }
    }

    void release_write_buffer()
    {
        m[1 - write_b].lock();
        idx[1 - write_b] = 0;
        m[write_b].unlock();
        write_b = 1 - write_b;
    }
};

}