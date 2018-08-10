/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

We have two types of objects in the simulation - Orrery bodies
(planets/moons/asteroid) and spaceships. They behave quite differently and
are described as separate classes of objects with some elements in common

*/

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "naifbody.hpp"
#include "vector.hpp"

namespace sim {

struct RockLike {

    struct Property {
        NAIFbody naif;

        float GM; // GM value for body
        float r;  // Radius of body (for collision tests)

        uint32_t color; // For display purposes
    };

    struct State {
        Vector pos; // Position referenced to solar system barycenter
        Vector vel; // Velocity
        double t_s;
    };
};

struct ShipLike {

    struct Property {
        NAIFbody naif;

        float max_acc;   // Maximum acceleration possible for ship m/s^2
        float max_fuel;  // Maximum fuel reserve (U)
        float burn_rate; // Fuel consumption rate (U/ (m/s^2))

        uint32_t color; // For display purposes
    };

    struct State {
        Vector pos;  // Position referenced to solar system barycenter
        Vector vel;  // Velocity
        Vector att;  // Attitude
        float  fuel; // Current fuel reserve (U)
        float  acc;  // Current acceleration km/s^2
        double t_s;
    };
};

template <typename T> struct SnapShot {
    T::Property property;
    T::State    state;
};

template <template <typename> class T> struct Objects {
    std::vector<T<RockLike>> system;
    std::vector<T<ShipLike>> fleet;
};

template <typename T>
inline std::optional<size_t>
find(const std::vector<T>& bodies, const NAIFbody& naif)
{
    for (size_t i = 0; i < bodies.size(); i++) {
        if (bodies[i].property.naif == naif) {
            return i;
        }
    }
    return {};
}
}
