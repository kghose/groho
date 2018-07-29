/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Commonly used functions in magnumapp
*/

#pragma once

#include <Magnum/Math/Color.h>

#include "style.hpp"

namespace sim {

inline Color3 from_rgb(unsigned long long rgb)
{
    return Color3::fromSrgb(
        Color3ub{ static_cast<unsigned char>((rgb >> 16) & 0xff),
                  static_cast<unsigned char>((rgb >> 8) & 0xff),
                  static_cast<unsigned char>(rgb & 0xff) });
}

inline Magnum::Vector3 v2v(Vector v)
{
    return Magnum::Vector3{ static_cast<float>(v.x / style::scale),
                            static_cast<float>(v.y / style::scale),
                            static_cast<float>(v.z / style::scale) };
}
}