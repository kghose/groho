#include "simulator.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

Simulator::Simulator(std::string result_file) { running = false; }

void Simulator::restart_with(const Scenario scenario_)
{
    if (scenario_.configuration) {
        running  = true;
        scenario = scenario_;

        begin_s = jd2s(scenario.configuration->begin_jd);
        end_s   = jd2s(scenario.configuration->end_jd);
        t_s     = begin_s;

        for (auto orrery_name : scenario.configuration->orrery) {
            orrery.load_orrery_model(orrery_name, begin_s, end_s);
        }

        LOG_S(INFO) << "Starting simulation";
    }
}

void Simulator::step()
{
    if (running) {
        if (t_s < end_s) {
            // pretend to do something
            auto obv = orrery.get_orrery_at(t_s);

            t_s += scenario.configuration->step;
        } else {
            running = false;
            LOG_S(INFO) << "Stopping simulation";
        }
    }
}

} // namespace sim