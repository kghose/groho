/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017 - 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#pragma once

#include <vector>

#include "naifbody.hpp"
#include "v3d.hpp"

namespace groho {

struct BodyConstant {
    const NAIFbody    code;
    const std::string name;
    const float       GM; // GM value for body
    const float       r;  // Radius of body (for collision tests)
};

// struct Body {
//     BodyConstant meta;
//     PosBuf       pos;
// };

// typedef std::vector<Body> body_vec_t;

}