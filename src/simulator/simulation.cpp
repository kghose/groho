/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This class collects the Orrery and the state history into a package that can
be shared between the Simulator and the Display.

It's important functions are:

* It carries the time history of a simulation
* It supplies const functions to the Display for interpolation on demand
* It implicitly caches the Orrery. The simulation is in charge of loading a
  new Orrery as needed, but the shared pointer mechanism ensures that a
  previously created Orrery remains (say if needed by a display showing
  an older simulation)
*/

#include "simulation.hpp"
#include "scenario.hpp"

namespace sim {

// TODO: Pass in pre-initialized snapshot to save compute?
RocksAndShips<SnapShot, SnapShot>
get_snapshot(double t_s, const RocksAndShips<Record, Record>& record)
{
    RocksAndShips<SnapShot, SnapShot> snapshot;

    for (size_t i = 0; i < record.system.size(); i++) {
        RockLike::State state;
        if (!record.system[i].property.naif.is_barycenter()) {
            state = record.system[i].history.at(t_s);
        }
        snapshot.system.push_back({ record.system[i].property, state });
    }

    for (size_t i = 0; i < record.fleet.size(); i++) {
        snapshot.fleet.push_back(
            { record.fleet[i].property, record.fleet[i].history.at(t_s) });
    }

    return snapshot;
}

Simulation::Simulation(
    const Configuration& new_config, std::shared_ptr<Simulation> old_simulation)
{
    if (old_simulation == nullptr) {
        simulation_serial = 1;

        orrery = load_orrery(new_config, config, orrery);

    } else {
        simulation_serial = old_simulation->simulation_serial + 1;

        orrery = load_orrery(
            new_config, old_simulation->config, old_simulation->orrery);
    }

    auto [record, rlock] = trajectory_data.borrow();

    for (auto& o : orrery->get_bodies()) {
        record.system.push_back({ o, {} });
    }

    config = new_config;

    auto [actions, alock] = action_list.borrow();

    int ship_code = -1000;
    // https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html#Spacecraft
    for (auto& fp_name : config.flightplan_fnames) {
        auto swa = load_ship(fp_name, --ship_code, record.fleet.size());
        if (swa) {
            record.fleet.push_back({ swa->ship.property, {} });
            actions.splice(actions.end(), swa->actions);
        }
    }
    actions.sort(fpa_order);

    LOG_S(INFO) << record.system.size() << " rocks in simulation.";
    LOG_S(INFO) << record.fleet.size() << " spaceships in simulation.";
    LOG_S(INFO) << "Loaded " << actions.size() << " actions";
}

// This conveniently generates a State object for the simulator with time
// initialized to whatever value we pass in
State Simulation::get_state_template(double init_t_s)
{
    auto [record, rlock] = trajectory_data.borrow();

    std::vector<RockLike::Property> rocks;
    for (auto const& r : record.system.bodies) {
        rocks.push_back(r.property);
    }

    std::vector<ShipLike::Property> ships;
    for (auto const& s : record.fleet.bodies) {
        ships.push_back(s.property);
    }

    return { init_t_s, rocks, ships };
}

// Add the current snapshot of simulation to the history
void Simulation::append(const State& state)
{
    auto [record, rlock] = trajectory_data.borrow();

    for (size_t i = 0; i < record.system.size(); i++) {
        if (!record.system[i].property.naif.is_barycenter()) {
            if (record.system[i].history.append(state.system[i].state())) {
                point_count++;
            }
        }
    }
    for (size_t i = 0; i < record.fleet.size(); i++) {
        if (record.fleet[i].history.append(state.fleet[i].state)) {
            point_count++;
        }
    }

    progress_s = state.t_s();
}

// Add any unsampled data into the history
bool Simulation::flush()
{
    auto [record, rlock] = trajectory_data.borrow();

    bool flushed = false;
    for (auto& r : record.system.bodies) {
        if (r.history.flush()) {
            flushed = true;
            point_count++;
        }
    }
    for (auto& s : record.fleet.bodies) {
        if (s.history.flush()) {
            flushed = true;
            point_count++;
        }
    }

    return flushed;
}

// // Gives the properties and state of the entire simulation at a given time
// // point. This is expensive because of the interpolation and copying
// void Simulation::get_snapshot_at(
//     double t_s, RocksAndShips<SnapShot>& snapshot) const
// {
//     for (size_t i = 0; i < system.size(); i++) {
//         if (!system[i].property.naif.is_barycenter()) {
//             snapshot.system[i].state = system[i].history.at(t_s);
//         }
//     }
//     for (size_t i = 0; i < fleet.size(); i++) {
//         snapshot.fleet[i].state = fleet[i].history.at(t_s);
//     }
// }

// BodyState Buffer::at(size_t i, double t_s) const
// {
//     std::lock_guard<std::mutex> lock(buffer_mutex);
//     return sub_buffer[i].at(t_s);
// }
}
