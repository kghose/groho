/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Collects style elements for the display
*/

#pragma once

#include <Magnum/Math/Color.h>

namespace sim {
namespace style {

using namespace Magnum;

const Color3 body_marker_color = Color3::yellow();
const Color3 trajectory_color  = Color3(0.3, 0.3, 0.8);

const double scale = 10 * 149'597'870.700; // 10 AU
}
}