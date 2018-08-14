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

    double begin_s() const;
    double end_s() const;

    std::shared_ptr<const Scenario> get_scenario() const { return scenario; }

public:
    enum Status { WAITING, LOADING, RUNNING };
    std::atomic<Status> status;

    std::shared_ptr<Scenario> scenario;

private:
    std::atomic<bool> running;

    std::thread compute_thread;
    // The simulation loop runs in this thread
};
}
