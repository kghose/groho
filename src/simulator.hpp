/*
  Handles the simulation part
*/
#pragma once

#include "scenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

// using namespace sim;
namespace sim {

class Simulator {
public:
    Simulator(std::string result_file) { running = false; }
    void restart_with(const Scenario scenario_)
    {
        if (scenario_.configuration) {
            running  = true;
            scenario = scenario_;

            jd = scenario.configuration->begin_jd;

            LOG_S(INFO) << "Starting simulation";
        }
    }
    void step();
    bool is_running() { return running; }

private:
    Scenario scenario;
    bool     running;
    double   jd;
};

void Simulator::step()
{
    if (running) {
        if (jd < scenario.configuration->end_jd) {
            // pretend to do something
            jd += scenario.configuration->step;
        } else {
            running = false;
            LOG_S(INFO) << "Stopping simulation";
        }
    }
}
}
