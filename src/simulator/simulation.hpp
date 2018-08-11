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

#include "buffer.hpp"
#include "state.hpp"

namespace sim {

struct SimulationSegment {
    NAIFbody naif;
    double   start_ts;
    double   end_ts;
};

template <typename T> struct Record {
    typename T::Property         property;
    SubBuffer<typename T::State> history;
};

struct Simulation : public Objects<Record> {

    //
    void append(const State&);

    // Add any unsampled data into the history
    bool flush();

    // We'll be writing to this and people will be clamoring to read from this
    // and we must have order!
    mutable std::mutex buffer_mutex;

    // This allows a reader to figure out if the simulation has been restarted
    // since their last read
    std::atomic<size_t> simulation_serial = 0;

    // This allows a reader to figure out if the data has changed since their
    // last read
    std::atomic<size_t> point_count = 0;
};

} // sim
