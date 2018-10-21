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
    simulation
        = std::shared_ptr<Simulation>(new Simulation(config, simulation));

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

void cleanup_actions(FlightPlans& flight_plans)
{
    for (auto& plan : flight_plans) {
        plan.remove_if([](std::unique_ptr<FlightPlanAction>& a) {
            DLOG_IF_S(INFO, a->done) << a->flightplan_fname << ":" << a->line_no
                                     << " : " << a->command_string << " done";
            return a->done;
        });
    }
}

void setup_actions(Simulation* simulation, State& state)
{
    auto [flight_plans, lock] = simulation->flightplans.borrow();

    for (size_t s = 0; s < flight_plans.size(); s++) {
        auto& ship = state.fleet[s];
        for (auto& action : flight_plans[s]) {
            action->setup(ship, state.system);
        }
    }

    cleanup_actions(flight_plans);
}

void execute_actions(Simulation* simulation, State& state)
{
    auto [flight_plans, fl_lock] = simulation->flightplans.borrow();
    auto [events, ev_lock]       = simulation->events.borrow();

    for (size_t s = 0; s < flight_plans.size(); s++) {
        auto& ship = state.fleet[s];
        for (auto& action : flight_plans[s]) {
            ShipCommand cmd = action->execute(ship, state.system);
            if (cmd.acc) {
                ship.state.acc = *(cmd.acc);
            }
            if (cmd.att) {
                ship.state.att = *(cmd.att);
            }
            if (cmd.event) {
                events.push_back(*(cmd.event));
            }

            if (action->is_blocking()) {
                break;
            }
        }
    }

    cleanup_actions(flight_plans);
}

void Simulator::run()
{
    if (!running)
        return;

    LOG_S(INFO) << "Starting simulation";

    State state = simulation->get_state_template(
        simulation->config.begin_s - simulation->config.step_s);

    // Prime the velocity computation
    simulation->orrery->set_body_positions(state.t_s(), state.system);
    state.advance_t_s(simulation->config.step_s);
    simulation->orrery->set_body_positions(state.t_s(), state.system);

    setup_actions(simulation.get(), state);

    status = RUNNING;
    while (running && (state.t_s() < simulation->config.end_s)) {

        simulation->orrery->set_body_positions(state.t_s(), state.system);

        update_ships(state, state.t_s(), simulation->config.step_s);
        execute_actions(simulation.get(), state);

        simulation->append(state);

        state.advance_t_s(simulation->config.step_s);
    }
    simulation->flush();
    running = false;
    status  = WAITING;

    LOG_S(INFO) << "Saved " << simulation->point_count << " state vectors";
    auto [events, ev_lock] = simulation->events.borrow();
    LOG_S(INFO) << "Saved " << events.size() << " events";

    LOG_S(INFO) << "Stopping simulation";
}

} // namespace sim