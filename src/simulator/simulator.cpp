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
    status   = LOADING;
    scenario = std::shared_ptr<Scenario>(new Scenario(config, scenario));
    if (!scenario->is_valid())
        return;

    running = true;

    // once more unto the breach
    compute_thread = std::thread(&sim::Simulator::run, std::ref(*this));
}

// TODO: Use leap frog integration
void update_ship(ShipLike::State& ship, const State& state, double step_s)
{
    Vector gravity{ 0, 0, 0 };

    for (auto& rock : state.system.bodies) {
        if (rock.property.naif.is_barycenter()) {
            continue;
        }

        auto r = rock.pos() - ship.pos;
        auto f = rock.property.GM / r.norm_sq();
        gravity += r.normed() * f;
    }

    ship.vel += ((ship.acc * ship.att) + gravity) * step_s;
    ship.pos += ship.vel * step_s;
}

void update_ships(State& state, double t_s, double step_s)
{
    for (auto& ship : state.fleet.bodies) {
        update_ship(ship.state, state, step_s);
        ship.state.t_s = t_s;
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
        if (state.t_s() < action->meta.t_s)
            continue;

        ShipCommand cmd = action->execute(state);
        if (cmd.acc) {
            state.fleet[action->meta.ship_idx].state.acc = *(cmd.acc);
        }
        if (cmd.att) {
            state.fleet[action->meta.ship_idx].state.att = *(cmd.att);
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

double Simulator::begin_s() const { return scenario->begin_s(); }

double Simulator::end_s() const { return scenario->end_s(); }

void Simulator::run()
{
    if (!running)
        return;

    LOG_S(INFO) << "Starting simulation";

    int   N = 0; // oscillates between 0 and 1
    State state(
        scenario->begin_s() - scenario->step_s(),
        scenario->simulation().system_proprty(),
        scenario->simulation().fleet_property());

    // Prime the velocity computation
    scenario->orrery()->set_body_positions(state.t_s(), state.system);
    state.advance_t_s(scenario->step_s());
    scenario->orrery()->set_body_positions(state.t_s(), state.system);

    setup_actions(scenario->actions, state);
    cleanup_actions(scenario->actions);

    status = RUNNING;
    while (running && (state.t_s() < scenario->end_s())) {
        scenario->orrery()->set_body_positions(state.t_s(), state.system);

        update_ships(state, state.t_s(), scenario->step_s());
        execute_actions(scenario->actions, state);
        cleanup_actions(scenario->actions);

        scenario->simulation().append(state);

        // buffer->append(state);

        state.advance_t_s(scenario->step_s());
    }
    scenario->simulation().flush();
    running = false;
    status  = WAITING;

    LOG_S(INFO) << "Saved " << scenario->simulation().point_count
                << " state vectors";
    LOG_S(INFO) << "Stopping simulation";
}

} // namespace sim