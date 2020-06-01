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

#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

namespace fs = std::filesystem;

const size_t buf_size = 5000;

template <typename T> class ThreadedBuffer {

public:
    ThreadedBuffer(fs::path fname)
    {
        buffer[0] = new T[buf_size];
        buffer[1] = new T[buf_size];
        idx[0] = idx[1] = 0;

        write_b = 0;
        file.open(fname, std::ios::binary | std::ios::out);

        saver = std::thread(&ThreadedBuffer::save_buffer_to_disk, this);
    }

    ~ThreadedBuffer()
    {
        wait_for(saved);
        run = false;
        set(go);
        saver.join();
        delete[] buffer[0];
        delete[] buffer[1];
    }

    void write(const T& k)
    {
        buffer[write_b][idx[write_b]] = k;
        idx[write_b]++;

        if (idx[write_b] == buf_size) {
            wait_for(saved);
            write_b      = 1 - write_b;
            idx[write_b] = 0;
            set(go);
        }
    }

private:
    std::ofstream file;
    T*            buffer[2];
    size_t        idx[2];
    size_t        write_b = 0;

    bool       go;
    bool       saved;
    bool       run = true;
    std::mutex m;

    std::condition_variable cv;

    std::thread saver;

    void save_buffer_to_disk()
    {
        set(saved);
        size_t save_b = 0;
        do {
            wait_for(go);
            file.write((char*)buffer[save_b], sizeof(T) * idx[save_b]);
            save_b = 1 - save_b;
            set(saved);
        } while (run);
    }

    void set(bool& v)
    {
        {
            std::unique_lock<std::mutex> lk(m);
            v = true;
        }
        cv.notify_one();
    }

    void wait_for(bool& v)
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&] { return v; });
        v = false;
    }
};

}