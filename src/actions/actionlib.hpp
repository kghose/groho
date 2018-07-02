/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Collection of methods useful for flight actions
*/

#pragma once

#include "vector.hpp"

namespace sim {

// Given GM of body, distance to body (R) and current velocity (Vs)
// compute the instantaneous deltaV we need to achieve a parking
// orbit at the current
Vector parking_deltaV(double GM, const Vector& R, const Vector& Vs);
}
