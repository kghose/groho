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

std::shared_ptr<const SpkOrrery> load_orrery(
    const Configuration&,
    const Configuration&,
    std::shared_ptr<const SpkOrrery>);

struct ShipWithActions {
    SnapShot<ShipLike> ship;
    fpapl_t            actions;
};

std::optional<ShipWithActions>
load_ship(std::string fp_name, int ship_code, int ship_idx);

}