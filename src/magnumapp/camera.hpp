/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This is a simple struct to store camera state. The state is passed to renderers
so that they know what camera model to use.
*/

#pragma once

#include "Magnum/Math/Matrix4.h"
#include <Magnum/Magnum.h>

namespace sim {

using namespace Magnum;
using namespace Math::Literals;

struct Camera {
    Deg   fov   = 35.0_degf;
    float front = 0.01f;
    float back  = 100.0f;
    float az    = 0.0f;
    float el    = 0.0f;

    Matrix4 transformation;
    Matrix4 projection;
};
}