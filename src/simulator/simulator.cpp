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
    status = WAITING;
}

void Simulator::restart_with(const Configuration& config)
{
    stop();
    status = LOADING;
    scenario.from(config);
    if (!scenario.valid)
        return;

    // TODO: move this into scenario loading ...
    LOG_S(INFO) << scenario.ships.size() << " spaceships in simulation.";

    buffer
        = std::shared_ptr<Buffer>(new Buffer(scenario, ++_simulation_serial));

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

void update_ships(
    std::vector<Body>&       ships,
    const std::vector<Body>& orrery,
    double                   t_s,
    double                   step_s)
{
    for (auto& ship : ships) {
        update_ship(ship, orrery, step_s);
        ship.state.t = t_s;
    }
}

void setup_actions(fpapl_t& actions, State& state)
{
    for (auto& action : actions) {
        action->setup(state);
    }
}

void execute_actions(fpapl_t& actions, State& state)
{
    for (auto& action : actions) {
        if (state.t_s < action->meta.t_s)
            continue;

        ShipCommand cmd = action->execute(state);
        if (cmd.acc) {
            state.ships[action->meta.ship_idx].state.acc = *(cmd.acc);
        }
        if (cmd.att) {
            state.ships[action->meta.ship_idx].state.att = *(cmd.att);
        }
    }
}

void cleanup_actions(fpapl_t& actions)
{
    actions.remove_if([](fpap_t& a) {
        DLOG_IF_S(INFO, a->done) << a->meta.fname << ":" << a->meta.line_no
                                 << " : " << a->meta.command_string << " done";
        return a->done;
    });
}

double Simulator::begin_s() const { return scenario.config.begin_s; }

double Simulator::end_s() const { return scenario.config.end_s; }

void Simulator::run()
{
    if (!running)
        return;

    LOG_S(INFO) << "Starting simulation";

    // TODO: deprecate this with a new mechanism for giving display a time
    // cursor
    t_s = scenario.config.begin_s;
    // The first run may involve actions that require the Orrery vel vectors
    // to be filled out
    State state(
        scenario.config.begin_s, scenario.orrery.get_orrery(), scenario.ships);

    scenario.orrery.set_orrery_with_vel_to(state.t_s);
    setup_actions(scenario.actions, state);
    cleanup_actions(scenario.actions);

    status = RUNNING;
    while (running && (state.t_s < scenario.config.end_s)) {
        scenario.orrery.set_orrery_to(state.t_s);

        update_ships(
            state.ships, state.orrery, state.t_s, scenario.config.step_s);
        execute_actions(scenario.actions, state);
        cleanup_actions(scenario.actions);
        buffer->append(state);

        state.t_s += scenario.config.step_s;
        t_s = state.t_s; // TODO: better mech for giving display a time cursor
    }
    buffer->flush();
    running = false;
    status  = WAITING;

    LOG_S(INFO) << "Saved " << buffer->point_count() << " state vectors";
    LOG_S(INFO) << "Stopping simulation";
}

std::shared_ptr<const Buffer> Simulator::get_buffer() const { return buffer; }

} // namespace sim