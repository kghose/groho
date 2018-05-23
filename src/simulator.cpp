/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file defines the simulator code
*/

#include "simulator.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

bool Simulator::time_range_changed(const Scenario& a, const Scenario& b)
{
    return (a.configuration->begin_jd != b.configuration->begin_jd)
        || (a.configuration->end_jd != b.configuration->end_jd);
}

bool Simulator::orrery_changed(const Scenario& a, const Scenario& b)
{
    return (a.configuration->orrery_fnames != b.configuration->orrery_fnames);
}

void Simulator::stop()
{
    running = false;
    if (compute_thread.joinable()) {
        compute_thread.join();
    }
}

void Simulator::restart_with(const Scenario scenario_)
{
    // TODO: Move the details of the check into scenario
    if (!scenario_.configuration)
        return;

    stop();

    // trying to figure out if we should reload the Orrery
    bool reload_orrery = !scenario.configuration
        || time_range_changed(scenario, scenario_)
        || orrery_changed(scenario, scenario_);

    scenario = scenario_;

    begin_s = jd2s(scenario.configuration->begin_jd);
    end_s   = jd2s(scenario.configuration->end_jd);
    t_s     = begin_s;
    step_s  = scenario.configuration->step;

    if (reload_orrery) {
        orrery = orrery::SpkOrrery();
        for (auto orrery_name : scenario.configuration->orrery_fnames) {
            orrery.load_orrery_model(orrery_name, begin_s, end_s);
        }
    }

    buffer = std::shared_ptr<Buffer>(new Buffer);
    buffer->lock();
    for (auto& o : orrery.get_orrery()) {
        buffer->add_body(Metadata{ o.code, o.name, o.color, o.real_body });
    }
    buffer->release();

    running = true;

    // once more unto the breach
    compute_thread = std::thread(&sim::Simulator::run, std::ref(*this));
}

void Simulator::run()
{
    if (!running)
        return;

    LOG_S(INFO) << "Starting simulation";

    while (running && (t_s < end_s)) {

        auto obv = orrery.get_orrery_at(t_s);
        buffer->lock();
        for (int i = 0; i < obv.size(); i++) {
            buffer->append(i, obv[i].pos);
        }
        buffer->release();
        t_s += step_s;
    }
    running = false;

    buffer->finalize();
    LOG_S(INFO) << "Stopping simulation";
}

} // namespace sim