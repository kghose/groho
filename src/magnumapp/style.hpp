/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Collects style elements for the display
*/

#pragma once

#include <Magnum/Math/Color.h>

namespace sim {

using namespace Magnum;

struct Style {
    Color3 canvas_color;
    Color3 overlay_color;
    Color3 body_marker_color;
    Color3 trajectory_color;

    Style(
        Color3 _canvas_color      = Color3(0.0, 0.0, 0.0),
        Color3 _overlay_color     = Color3(1.0, 1.0, 1.0),
        Color3 _body_marker_color = Color3(1.0, 1.0, 1.0),
        Color3 _trajectory_color  = Color3(0.8, 0.8, 0.8))
        : canvas_color(_canvas_color)
        , overlay_color(_overlay_color)
        , body_marker_color(_body_marker_color)
        , trajectory_color(_trajectory_color)
    {
        ;
    }
};

namespace style {

const double scale = 10 * 149'597'870.700; // 10 AU
}
}