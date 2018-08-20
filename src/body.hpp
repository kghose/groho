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
template <typename T> struct SnapShotV {

    SnapShotV(int& N, typename T::Property _property)
        : _N(N)
    {
        property = _property;
    }

    typename T::Property property;
    typename T::State    _state[2];

    double& t_s() { return _state[_N].t_s; }
    double  t_s() const { return _state[_N].t_s; }

    Vector&       pos() { return _state[_N].pos; }
    const Vector& pos() const { return _state[_N].pos; }
    Vector        vel() const
    {
        return (_state[_N].pos - _state[1 - _N].pos)
            / (_state[_N].t_s - _state[1 - _N].t_s);
    }

    const typename T::State& state() const { return _state[_N]; }

    int& _N;
};

template <typename T> struct BaseCollection {
    std::vector<T>                       bodies;
    std::unordered_map<NAIFbody, size_t> lookup;

    size_t size() const { return bodies.size(); }

    T& operator[](size_t idx) { return bodies[idx]; }
    T& operator[](const NAIFbody& id) { return bodies[lookup[id]]; }

    const T& operator[](size_t idx) const { return bodies[idx]; }
    const T& operator[](const NAIFbody& id) const { return bodies[lookup[id]]; }

    virtual void push_back(const T& body)
    {
        bodies.push_back(body);
        lookup[body.property.naif] = bodies.size() - 1;
    }
};

template <typename T> struct Collection : public BaseCollection<T> {
};

// template <>
template <typename T>
struct Collection<SnapShotV<T>> : public BaseCollection<SnapShotV<T>> {

    int N = 0;

    void push_back(const typename T::Property& property)
    {
        // using BaseCollection<SnapShotV<T>>::bodies;
        // using BaseCollection<SnapShotV<T>>::lookup;

        bodies.push_back({ N, property });
        lookup[property.naif] = bodies.size() - 1;
    }

private:
    using BaseCollection<SnapShotV<T>>::push_back;
};

template <template <typename> class Trock, template <typename> class Tship>
struct RocksAndShips {
    Collection<Trock<RockLike>> system;
    Collection<Tship<ShipLike>> fleet;
};

/*
// TODO: deprecate
// This allows us to do lazy computation of rock velocities.
// Time will tell if we were too clever here
struct RockSnapShotWithVel {

    RockSnapShotWithVel(int& N, RockLike::Property _property)
        : _N(N)
    {
        property = _property;
    }

    RockLike::Property property;
    RockLike::State    _state[2];

    double& t_s() { return _state[_N].t_s; }
    double  t_s() const { return _state[_N].t_s; }

    Vector&       pos() { return _state[_N].pos; }
    const Vector& pos() const { return _state[_N].pos; }
    Vector        vel() const
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
*/

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
