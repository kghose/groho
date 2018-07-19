/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Renders a text label
*/

#pragma once

#include <Corrade/PluginManager/Manager.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Complex.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>
#include <iomanip>
#include <sstream>

#include "camera.hpp"
#include "font.hpp"

namespace sim {

using namespace Magnum;

class TextLabel {

public:
    TextLabel(Font& font);

    TextLabel& set_text(std::string str);
    TextLabel& set_color(Color3 col);
    TextLabel& set_pos(Vector3d p);

    void draw(const Camera& camera);

private:
    std::unique_ptr<Text::Renderer2D> _text2;
    Shaders::DistanceFieldVector2D    _shader;

    Matrix3 _transformation;
    Matrix3 _projection;

    Color3 _color;
};
}