/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file defines the simulator code
*/

#include "simulator.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

bool time_range_extended(const Scenario& a, const Scenario& b)
{
    return (b.configuration->begin_jd < a.configuration->begin_jd)
        || (a.configuration->end_jd < b.configuration->end_jd);
}

bool orrery_changed(const Scenario& a, const Scenario& b)
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
        || time_range_extended(scenario, scenario_)
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

    LOG_S(INFO) << scenario.ships.size() << " spaceships in simulation.";

    buffer = std::shared_ptr<Buffer>(new Buffer);
    buffer->set_simulation_serial(++_simulation_serial);

    buffer->lock();
    // buffer contains orrery bodies followed by spaceships
    for (auto& o : orrery.get_orrery()) {
        buffer->add_body(o);
    }
    for (auto& ship : scenario.ships) {
        buffer->add_body(ship.body);
    }
    buffer->release();

    // Now do any spaceship initializations that requires world state
    WorldState ws(t_s, orrery.get_orrery_with_vel_at(t_s));
    for (auto& ship : scenario.ships) {
        ship.init(ws);
    }

    running = true;

    // once more unto the breach
    compute_thread = std::thread(&sim::Simulator::run, std::ref(*this));
}

// TODO: Use leap frog integration
void step_spaceship(
    Body& spaceship, const orrery::OrreryBodyVec& obv, double step_s)
{
    Vector gravity{ 0, 0, 0 };
    for (auto& o : obv) {

        if (o.body_type == BARYCENTER) {
            continue;
        }

        auto r = o.pos - spaceship.pos;
        auto f = o.GM / r.norm_sq();
        gravity += r.normed() * f;
    }
    spaceship.vel += ((spaceship.acc * spaceship.att) + gravity) * step_s;
    spaceship.pos += spaceship.vel * step_s;
}

void execute_flight_plan(Ship& ship, const WorldState& ws, double t_s)
{
    for (auto& action : ship.plan) {
        if (action->active && (t_s >= action->t_s)) {
            (*action)(ship.body, ws);
        }
    }
}

void Simulator::run()
{
    if (!running)
        return;

    LOG_S(INFO) << "Starting simulation";

    while (running && (t_s < end_s)) {

        WorldState ws(t_s, orrery.get_orrery_at(t_s));

        for (auto& ship : scenario.ships) {
            step_spaceship(ship.body, ws.obv, step_s);
            execute_flight_plan(ship, ws, t_s);
        }

        bool final_step = t_s >= end_s - step_s;

        buffer->lock();

        // First do the orrery
        for (int i = 0; i < ws.obv.size(); i++) {
            buffer->append(i, ws.obv[i].pos, final_step);
        }
        // Then the spaceships
        for (int i = 0; i < scenario.ships.size(); i++) {
            buffer->append(
                i + ws.obv.size(), scenario.ships[i].body.pos, final_step);
        }

        buffer->release();
        t_s += step_s;
    }
    running = false;
    buffer->finalize();

    LOG_S(INFO) << "Stopping simulation";
}

} // namespace sim