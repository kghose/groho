/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

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

#include <forward_list>
#include <optional>
#include <string>
#include <vector>

#include "body.hpp"
#include "buffer.hpp"
#include "configuration.hpp"
#include "flightplanaction.hpp"
#include "simulation.hpp"
#include "spkorrery.hpp"
#include "state.hpp"

namespace sim {

// This class is a bit "insulated" because we want it's contents modified only
// in certain principled ways even by components (like the simulator) that we
// expect to have write access.
class Scenario {

public:
    // We use the old Scenario in order to do any caching that we can
    Scenario(const Configuration&, std::shared_ptr<Scenario>);

    // If we have errors on construction, we flag this
    bool is_valid() { return _valid; }

    // Allows the simulator to save simulation data into history
    void append(const State& state) { _simulation.append(state); }

    // Gives the properties and state of the entire simulation at a given time
    // point. This is expensive because of the interpolation and copying
    void get_snapshot_at(double t_s, Objects<SnapShot>&) const;

    // The passed in function has the opportunity to copy over the simulation
    void read(
        const std::vector<SimulationSegment>&,
        std::function<void(const Simulation&)>) const;

    // Possibly very expensive operation. A display element can ask for a
    // simulation object that only consists of parts of the simulation and that
    // has been interpolated
    void interpolate(
        const std::vector<SimulationSegment>&,
        std::function<void(const Simulation&)>);

    // This allows a reader to figure out if the simulation has been restarted
    // since their last read
    constexpr size_t simulation_serial() const
    {
        return _simulation.simulation_serial;
    }

    constexpr const Simulation& simulation() { return _simulation; }

    // This allows a reader to figure out if the data has changed since their
    // last read
    constexpr size_t point_count() const { return _simulation.point_count; }

    constexpr const Configuration& config() const { return _config; }

    std::shared_ptr<const SpkOrrery> orrery() const { return _orrery; }

private:
    // We want to construct a scenario and then have the simulator iterate on it
    // While it is cheap to load configurations and flightplans, orreries can
    // be expensive to both load and keep around, so we do a form of caching by
    // using shared pointers
    std::shared_ptr<const SpkOrrery> _orrery;

    Simulation _simulation;
    fpapl_t    _actions;

    Configuration _config;

    // TODO: more fine grained errors?
    bool _valid;
};
}