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

namespace sim {

void Simulation::append(const State& state)
{
    std::lock_guard<std::mutex> lock(buffer_mutex);

    for (size_t i = 0; i < system.size(); i++) {
        if (!system[i].property.naif.is_barycenter()) {
            system[i].history.append(state.system()[i]);
        }
    }

    for (size_t i = 0; i < state.fleet.size(); i++) {
        fleet[i].history.append(state.fleet()[i]);
    }
}

// Add any unsampled data into the history
bool Simulation::flush()
{
    std::lock_guard<std::mutex> lock(buffer_mutex);

    bool flushed = false;
    for (auto& r : system) {
        if (r.history.flush()) {
            flushed = true;
            point_count++;
        }
    }
    for (auto& s : fleet) {
        if (s.history.flush()) {
            flushed = true;
            point_count++;
        }
    }

    return flushed;
}

// BodyState Buffer::at(size_t i, double t_s) const
// {
//     std::lock_guard<std::mutex> lock(buffer_mutex);
//     return sub_buffer[i].at(t_s);
// }
}
