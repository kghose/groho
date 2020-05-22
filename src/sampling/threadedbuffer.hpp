/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Double buffer class for writing out to disk. (See my blog post:
https://kaushikghose.wordpress.com/2020/03/20/buffering-writes/)

Example usage:

int main(int argc, char* argv[])
{
    ThreadedBuffer buffer("test.bin");
    for (size_t j = 0; j < (1 << 27); j++) {
        size_t k = (size_t)(j * 1.5 - j * 1.1 + j / 3.2);
        buffer.write(k);
    }
}*/

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

template <typename T> class ThreadedBuffer {
    enum BufferId { one = 0, two = 1, neither = 3 };

public:
    ThreadedBuffer(fs::path fname)
    {
        _buffer[0]  = new T[buf_size];
        _buffer[1]  = new T[buf_size];
        _buf_idx[0] = _buf_idx[1] = 0;

        _buffer_to_write_to   = BufferId::one;
        _buffer_being_flushed = BufferId::neither;
        file.open(fname, std::ios::binary | std::ios::out);

        ready_to_write = false;
        processed      = false;
        _writer        = std::thread(&ThreadedBuffer::_write_out_buffer, this);
    }

    ~ThreadedBuffer()
    {
        if (_buffer_being_flushed != BufferId::neither) {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&] { return processed; });
        }

        _buffer_being_flushed = _buffer_to_write_to;
        processed             = false;
        ready_to_write        = true;
        cv.notify_one();

        keep_running = false;

        {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&] { return processed; });
        }

        _writer.join();
        delete[] _buffer[0];
        delete[] _buffer[1];
    }

    void write(const T& k)
    {
        _buffer[_buffer_to_write_to][_buf_idx[_buffer_to_write_to]] = k;
        _buf_idx[_buffer_to_write_to]++;

        if (_buf_idx[_buffer_to_write_to] == buf_size) {

            BufferId new_buffer_to_write_to
                = _buffer_to_write_to == BufferId::one ? BufferId::two
                                                       : BufferId::one;

            if (_buffer_being_flushed == new_buffer_to_write_to) {
                std::unique_lock<std::mutex> lk(m);
                cv.wait(lk, [&] { return processed; });
            }

            _buffer_being_flushed         = _buffer_to_write_to;
            _buffer_to_write_to           = new_buffer_to_write_to;
            _buf_idx[_buffer_to_write_to] = 0;

            std::lock_guard<std::mutex> lk(m);
            processed      = false;
            ready_to_write = true;
            cv.notify_one();
        }
    }

private:
    std::ofstream file;
    T*            _buffer[2];
    size_t        _buf_idx[2];

    BufferId    _buffer_to_write_to;
    BufferId    _buffer_being_flushed;
    std::thread _writer;

    std::atomic<bool> keep_running = true;
    std::mutex        m;

    std::condition_variable cv;

    bool ready_to_write = false;
    bool processed      = false;

    void _write_out_buffer()
    {
        while (keep_running) {

            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&] { return ready_to_write; });

            file.write(
                (char*)_buffer[_buffer_being_flushed],
                sizeof(size_t) * _buf_idx[_buffer_being_flushed]);

            _buf_idx[_buffer_being_flushed] = 0;

            _buffer_being_flushed = BufferId::neither;

            processed      = true;
            ready_to_write = false;

            lk.unlock();
            cv.notify_one();
        }
    }
};
}