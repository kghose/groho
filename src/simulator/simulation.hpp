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

#pragma once

#include <functional>
#include <memory>

#include "configuration.hpp"
#include "flightplanaction.hpp"
#include "lockable.hpp"
#include "sampledhistory.hpp"
#include "spkorrery.hpp"
#include "state.hpp"

namespace sim {

struct SimulationSegment {
    NAIFbody naif;
    double   start_ts;
    double   end_ts;
};

template <typename T> struct Record {
    typename T::Property              property;
    SampledHistory<typename T::State> history;
};

RocksAndShips<SnapShot, SnapShot>
get_snapshot(double, const RocksAndShips<Record, Record>&);

class Simulation {
public:
    // We use the old Simulation in order to do any caching that we can
    Simulation(const Configuration&, std::shared_ptr<Simulation>);

    // This conveniently generates a State object for the simulator with time
    // initialized to whatever value we pass in
    State get_state_template(double);

    // Add the current snapshot of simulation to the history
    void append(const State&);

    // Add any unsampled data into the history
    bool flush();

    // This allows a reader to figure out if the simulation has been restarted
    // since their last read
    std::atomic<size_t> simulation_serial = 0;

    // This allows a reader to figure out if the data has changed since their
    // last read
    std::atomic<size_t> point_count = 0;

    Configuration config;

    // We want to construct a scenario and then have the simulator iterate on it
    // While it is cheap to load configurations and flightplans, orreries can
    // be expensive to both load and keep around, so we do a form of caching by
    // using shared pointers
    std::shared_ptr<const SpkOrrery> orrery;

    // The data store!
    Lockable<RocksAndShips<Record, Record>> trajectory_data;

    // Flightplan actions
    Lockable<fpapl_t> action_list;
};

} // sim
