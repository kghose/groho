/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

We have two types of objects in the simulation - Orrery bodies
(planets/moons/asteroid) and spaceships. They have some metadata in common and
some that are unique.

To simplify our first iteration we combine all the metadata and state
information and use a unifed system with a type flag to indicate what type of
object we are refering to. This determines which fields make sense to access.
*/

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "spkid.hpp"
#include "vector.hpp"

namespace sim {

enum BodyType { ROCK, BARYCENTER, SPACESHIP };
enum FlightState { FALLING, LANDED };

struct BodyConstant {
    BodyType    body_type;
    int         code; // SPK code for body (made up ones for spaceships)
    std::string name; // Human readable name for body

    float GM; // GM value for body
    float r;  // Radius of body (for collision tests)

    uint32_t color; // For display purposes
};

struct ShipCharacteristic {
    float max_acc;   // Maximum acceleration possible for ship m/s^2
    float max_fuel;  // Maximum fuel reserve (U)
    float burn_rate; // Fuel consumption rate (U/ (m/s^2))
};

struct BodyState {
    double t;

    Vector pos; // Position referenced to solar system barycenter
    Vector vel; // Velocity
    Vector att; // Attitude

    float acc;  // Current acceleration km/s^2
    float fuel; // Current fuel reserve (U)

    FlightState flight_state;
};

struct Body {
    BodyConstant       property;
    ShipCharacteristic param;
    BodyState          state;
};

inline std::optional<size_t>
body_index(const std::vector<Body>& bodies, spkid_t spkid)
{
    for (size_t i = 0; i < bodies.size(); i++) {
        if (bodies[i].property.code == spkid.id) {
            return i;
        }
    }
    return {};
}
}
