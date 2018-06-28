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