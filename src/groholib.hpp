/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Commonly used utility functions
*/

#pragma once

#include <optional>
#include <vector>

#include "body.hpp"
#include "groho.hpp"

namespace sim {

const double T0        = 2451545.0;
const double S_PER_DAY = 86400.0;

inline double jd2s(double jd) { return (jd - T0) * S_PER_DAY; }
inline double s2jd(double s) { return T0 + s / S_PER_DAY; }

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