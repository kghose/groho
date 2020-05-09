/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

A small class to carry a Vector data structure
*/

#pragma once

#include <cmath>
#include <iostream>

namespace groho {

struct V3d {
    double x, y, z;

    double norm_sq() const { return x * x + y * y + z * z; }
    double norm() const { return std::sqrt(norm_sq()); }
    V3d    normed() const { return V3d(*this) / norm(); }

    V3d operator+(const V3d& rhs) const
    {
        return V3d{ x + rhs.x, y + rhs.y, z + rhs.z };
    }

    V3d operator-(const V3d& rhs) const
    {
        return V3d{ x - rhs.x, y - rhs.y, z - rhs.z };
    }

    V3d operator/(const double rhs) const
    {
        return V3d{ x / rhs, y / rhs, z / rhs };
    }

    V3d& operator+=(const V3d& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
};

typedef std::vector<V3d> v3d_vec_t;

inline double dot(const V3d& u, const V3d& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline V3d operator*(const V3d& u, const V3d& v)
{
    return { u.y * v.z - u.z * v.y,
             u.z * v.x - u.x * v.z,
             u.x * v.y - u.y * v.x };
}

inline V3d cross(const V3d& u, const V3d& v) { return u * v; }

inline V3d operator*(double lhs, const V3d& rhs)
{
    return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}

inline V3d operator*(const V3d& lhs, double rhs)
{
    return { rhs * lhs.x, rhs * lhs.y, rhs * lhs.z };
}

inline V3d rotate(const V3d& v, V3d k, double th)
// Rotate v about k by th
// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
{
    k = k / k.norm();
    return v * std::cos(th) + cross(k, v) * std::sin(th)
        + k * dot(k, v) * (1 - std::cos(th));
}

inline std::ostream& operator<<(std::ostream& os, const V3d& v)
{
    os << v.x << ", " << v.y << ", " << v.z;
    return os;
}

// Rolling buffer storing the last three position values
class PosBuf {
public:
    void append(V3d& p)
    {
        _pos[_idx] = p;
        _idx++;
        if (_idx > 2) {
            _idx = 0;
        }
    }

    V3d& T0() { return _pos[_idx]; }

    const V3d& T1() const
    {
        switch (_idx) {
        case 0:
            return _pos[2];
            break;
        case 1:
            return _pos[0];
            break;
        case 2:
            return _pos[1];
            break;
        }
    }

    const V3d& T2() const
    {
        switch (_idx) {
        case 0:
            return _pos[1];
            break;
        case 1:
            return _pos[2];
            break;
        case 2:
            return _pos[0];
            break;
        }
    }

private:
    V3d    _pos[3];
    size_t _idx = 0;
};

struct LatLon {
    float lat, lon;
    // LatLon( float _lat, float _lon ) { lat = _lat; lon = _lon; }
};

inline std::ostream& operator<<(std::ostream& os, const LatLon& ll)
{
    os << ll.lat << ", " << ll.lon;
    return os;
}

} // namespace groho