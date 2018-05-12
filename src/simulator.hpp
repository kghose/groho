/*
  Handles the simulation part
*/
#pragma once

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
    Simulator(std::string result_file);
    void restart_with(const Scenario scenario_);
    void step();
    bool is_running() { return running; }

private:
    Scenario scenario;

    orrery::SpkOrrery orrery;

    bool   running;
    double t_s;
    double begin_s;
    double end_s;
};
}
