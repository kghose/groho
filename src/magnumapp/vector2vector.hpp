/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Converts a sim::Vector into a Magnum::Vector3
*/

#pragma once

#include <Magnum/Math/Matrix4.h>

#include "groho.hpp"
#include "vector.hpp"

namespace sim {

inline Magnum::Vector3 v2v(Vector v)
{
    return Magnum::Vector3{ static_cast<float>(v.x / globals::scale),
                            static_cast<float>(v.y / globals::scale),
                            static_cast<float>(v.z / globals::scale) };
}
}
