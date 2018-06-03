/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

A small class to carry a Vector data structure
*/

#pragma once

#include <cmath>
#include <iostream>

namespace sim {

struct Vector {
    double x, y, z;

    double norm_sq() const { return x * x + y * y + z * z; }
    double norm() const { return std::sqrt(norm_sq()); }
    Vector normed() const { return Vector(*this) / norm(); }

    Vector operator+(const Vector& rhs) const
    {
        return Vector{ x + rhs.x, y + rhs.y, z + rhs.z };
    }

    Vector operator-(const Vector& rhs) const
    {
        return Vector{ x - rhs.x, y - rhs.y, z - rhs.z };
    }

    Vector operator/(const double rhs) const
    {
        return Vector{ x / rhs, y / rhs, z / rhs };
    }

    Vector& operator+=(const Vector& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
};

inline double dot(const Vector& u, const Vector& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline Vector operator*(const Vector& u, const Vector& v)
{
    return { u.y * v.z - u.z * v.y,
             u.z * v.x - u.x * v.z,
             u.x * v.y - u.y * v.x };
}

inline Vector cross(const Vector& u, const Vector& v) { return u * v; }

inline Vector operator*(double lhs, const Vector& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}

inline Vector operator*(const Vector& lhs, double rhs)
{
    return { rhs * lhs.x, rhs * lhs.y, rhs * lhs.z };
}

inline Vector rotate(const Vector& v, Vector k, double th)
// Rotate v about k by th
// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
{
    k = k / k.norm();
    return v * std::cos(th) + cross(k, v) * std::sin(th)
        + k * dot(k, v) * (1 - std::cos(th));
}

inline std::ostream& operator<<(std::ostream& os, const Vector& v)
{
    os << v.x << ", " << v.y << ", " << v.z;
    return os;
}

struct LatLon {
    float lat, lon;
    // LatLon( float _lat, float _lon ) { lat = _lat; lon = _lon; }
};

inline std::ostream& operator<<(std::ostream& os, const LatLon& ll)
{
    os << ll.lat << ", " << ll.lon;
    return os;
}

} // namespace sim