/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file declares the simulator code
*/

#pragma once

#include <thread>

#include "buffer.hpp"
#include "scenario.hpp"
#include "spkorrery.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

// using namespace sim;
namespace sim {

class Simulator {
public:
    Simulator() { running = false; }
    void restart_with(const Scenario scenario_);
    void run();
    void stop();

    std::shared_ptr<const Buffer> get_buffer() const { return buffer; }

private:
    bool time_range_changed(const Scenario& a, const Scenario& b);
    bool orrery_changed(const Scenario& a, const Scenario& b);

    Scenario          scenario;
    orrery::SpkOrrery orrery;

    std::shared_ptr<Buffer>   buffer;
    std::atomic<unsigned int> _simulation_serial = 0;

    std::atomic<bool> running;

    double t_s;
    double step_s;
    double begin_s;
    double end_s;

    std::thread compute_thread;
    // The simulation loop runs in this thread
};
}
