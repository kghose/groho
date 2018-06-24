/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file defines the simulator code
*/

#include "simulator.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

void Simulator::stop()
{
    running = false;
    if (compute_thread.joinable()) {
        compute_thread.join();
    }
}

void Simulator::restart_with(const Configuration& config)
{
    stop();
    scenario.from(config);
    if (!scenario.valid)
        return;

    LOG_S(INFO) << scenario.ships.size() << " spaceships in simulation.";

    buffer = std::shared_ptr<Buffer>(new Buffer);
    buffer->set_simulation_serial(++_simulation_serial);

    buffer->lock();
    // buffer contains orrery bodies followed by spaceships
    for (auto& o : scenario.orrery.get_orrery()) {
        buffer->add_body(o);
    }
    for (auto& ship : scenario.ships) {
        buffer->add_body(ship);
    }
    buffer->release();

    running = true;

    // once more unto the breach
    compute_thread = std::thread(&sim::Simulator::run, std::ref(*this));
}

// TODO: Use leap frog integration
void update_ship(Body& ship, const std::vector<Body>& orrery, double step_s)
{
    Vector gravity{ 0, 0, 0 };

    for (auto& o : orrery) {

        if (o.property.body_type == BARYCENTER) {
            continue;
        }

        auto r = o.state.pos - ship.state.pos;
        auto f = o.property.GM / r.norm_sq();
        gravity += r.normed() * f;
    }

    ship.state.vel += ((ship.state.acc * ship.state.att) + gravity) * step_s;
    ship.state.pos += ship.state.vel * step_s;
}

void update_ships(State& state, double step_s)
{
    for (auto& ship : state.ships) {
        update_ship(ship, state.orrery, step_s);
        ship.state.t = state.t_s;
    }
}

void execute_actions(fpapl_t& actions, State& state)
{
    for (auto& action : actions) {
        (*action)(state);
    }

    actions.remove_if([](fpap_t& a) { return a->done; });
}

void Simulator::run()
{
    if (!running)
        return;

    LOG_S(INFO) << "Starting simulation";

    // TODO: deprectae this with a new mechnism for giving display a time cursor
    t_s = scenario.config.begin_s;
    // The first run may involve actions that require the Orrery vel vectors
    // to be filled out
    State state(
        scenario.config.begin_s,
        scenario.orrery.get_orrery_with_vel_at(scenario.config.begin_s),
        scenario.ships);

    while (running && (state.t_s < scenario.config.end_s)) {

        update_ships(state, scenario.config.step_s);
        execute_actions(scenario.actions, state);

        buffer->lock();

        // First do the orrery
        for (int i = 0; i < state.orrery.size(); i++) {
            buffer->append(i, state.orrery[i].state);
        }
        // Then the spaceships
        for (int i = 0; i < state.ships.size(); i++) {
            buffer->append(i + state.orrery.size(), state.ships[i].state);
        }

        buffer->release();

        state.t_s += scenario.config.step_s;
        t_s = state.t_s; // TODO: better mech for giving display a time cursor

        scenario.orrery.get_orrery_at(state.t_s);
    }
    buffer->flush();
    running = false;

    LOG_S(INFO) << "Saved " << buffer->point_count() << " state vectors";
    LOG_S(INFO) << "Stopping simulation";
}

std::shared_ptr<const Buffer> Simulator::get_buffer() const
{
    if (buffer) {
        buffer->flush();
    }
    return buffer;
}

} // namespace sim