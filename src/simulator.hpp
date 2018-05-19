/*
  Handles the simulation part
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

const double T0        = 2451545.0;
const double S_PER_DAY = 86400.0;

inline double jd2s(double jd) { return (jd - T0) * S_PER_DAY; }

class Simulator {
public:
    Simulator() { running = false; }
    void restart_with(const Scenario scenario_);
    void run();
    void stop();

private:
    bool time_range_changed(const Scenario& a, const Scenario& b);
    bool orrery_changed(const Scenario& a, const Scenario& b);

    Scenario scenario;

    orrery::SpkOrrery orrery;

    std::string result_file;

    Buffer buffer;

    std::atomic<bool> running;

    double t_s;
    double step_s;
    double begin_s;
    double end_s;

    std::thread compute_thread;
    // The simulation loop runs in this thread
};
}
