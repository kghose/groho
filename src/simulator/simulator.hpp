/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file declares the simulator code
*/

#pragma once

#include <atomic>
#include <forward_list>
#include <memory>
#include <thread>

//#include "action.hpp"
#include "buffer.hpp"
#include "scenario.hpp"
#include "spkorrery.hpp"
#include "state.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

// using namespace sim;
namespace sim {

class Simulator {
public:
    Simulator()
    {
        running = false;
        status  = WAITING;
    }
    void restart_with(const Configuration&);
    void run();
    void stop();

    std::shared_ptr<const Buffer> get_buffer() const;

public:
    enum Status { WAITING, LOADING, RUNNING };
    std::atomic<Status> status;

    // TODO: this is used by display to find latest time cursor
    // this is in-elegant. Let's change this to a better mechanism
    double t_s;

private:
    Scenario scenario;

    std::shared_ptr<Buffer>   buffer;
    std::atomic<unsigned int> _simulation_serial = 0;

    std::atomic<bool> running;

    // double step_s;
    // double begin_s;
    // double end_s;

    std::thread compute_thread;
    // The simulation loop runs in this thread
};
}
