/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Code to parse scneario files and flight plans and load in the Orrery and Ship
definitions.
*/

#pragma once

#include <forward_list>
#include <optional>
#include <string>
#include <vector>

#include "configuration.hpp"
#include "flightplanaction.hpp"
#include "spkorrery.hpp"

namespace sim {

std::shared_ptr<const SpkOrrery> load_orrery(
    const Configuration&,
    const Configuration&,
    std::shared_ptr<const SpkOrrery>);

struct LoadedShip {
    ShipLike::Property property;
    FlightPlan         flight_plan;
};

std::optional<LoadedShip> load_ship(std::string fp_name, int ship_code);
}