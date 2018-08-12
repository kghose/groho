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
#include <unordered_map>
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
    typename T::Property property;
    typename T::State    state;
};

// This allows us to do lazy computation of rock velocities.
// Time will tell if we were too clever here
struct RockSnapShotWithVel {

    RockSnapShotWithVel(int& N)
        : _N(N)
    {
    }

    RockSnapShotWithVel(const RockSnapShotWithVel& _ssv)
        : _N(_ssv._N)
    {
        property  = _ssv.property;
        _state[0] = _ssv._state[0];
        _state[1] = _ssv._state[1];
    }

    RockSnapShotWithVel(int& N, RockLike::Property _property)
        : _N(N)
    {
        property = _property;
    }

    RockLike::Property property;
    RockLike::State    _state[2];

    constexpr double& t_s() { return _state[_N].t_s; }
    constexpr double  t_s() const { return _state[_N].t_s; }

    constexpr Vector&       pos() { return _state[_N].pos; }
    constexpr const Vector& pos() const { return _state[_N].pos; }
    constexpr Vector        vel() const
    {
        return (_state[_N].pos - _state[1 - _N].pos)
            / (_state[_N].t_s - _state[1 - _N].t_s);
    }

    constexpr const RockLike::State& state() const { return _state[_N]; }

    int& _N;
};

template <typename T> struct BaseCollection {
    std::vector<T>                       bodies;
    std::unordered_map<NAIFbody, size_t> lookup;

    constexpr size_t size() const { return bodies.size(); }

    constexpr T& operator[](size_t idx) { return bodies[idx]; }
    constexpr T& operator[](const NAIFbody& id) { return bodies[lookup[id]]; }

    constexpr const T& operator[](size_t idx) const { return bodies[idx]; }

    // Collection(const Collection<T>& _collection) = default;

    // template <typename T2> Collection(const Collection<T2>& _collection)
    // {
    //     bodies = _collection.bodies;
    //     lookup = _collection.lookup;
    // }

    virtual void push_back(const T& body)
    {
        bodies.push_back(body);
        lookup[body.property.naif] = bodies.size() - 1;
    }
};

template <typename T> struct Collection : public BaseCollection<T> {
};

template <>
struct Collection<RockSnapShotWithVel>
    : public BaseCollection<RockSnapShotWithVel> {

    int N = 0;

    void push_back(const RockLike::Property& property)
    {
        bodies.push_back({ N, property });
        lookup[property.naif] = bodies.size() - 1;
    }

private:
    using BaseCollection<RockSnapShotWithVel>::push_back;
};

template <template <typename> class T> struct RocksAndShips {
    Collection<T<RockLike>> system;
    Collection<T<ShipLike>> fleet;
};

// struct RocksAndShipsSnapShotWithVel {

//     int N = 0;

//     Collection<RockSnapShotWithVel> system;
//     Collection<SnapShot<ShipLike>>  fleet;

//     RocksAndShipsSnapShotWithVel() = default;
// };

// TODO: DEPRECATE THIS
template <template <typename> class T> struct Objects {
    std::vector<T<RockLike>> system;
    std::vector<T<ShipLike>> fleet;

    std::unordered_map<NAIFbody, size_t> lookup_system;
    std::unordered_map<NAIFbody, size_t> lookup_fleet;
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
