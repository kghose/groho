/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing data to display for a given object
*/

#pragma once

#include <vector>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>

#include "body.hpp"
#include "groho.hpp"
#include "vector.hpp"
#include "vector2vector.hpp"

namespace sim {

using namespace Magnum;
using namespace Corrade;

class Path {

public:
    void set_color(Color3 color) { _color = color; }

    // This overwrites the existing data
    void set_data(const std::vector<BodyState>& state);

    // This adds on the new elements of "state" to the display buffer.
    // Raises runtime error if "state" is smaller than the display buffer
    // because this is a sure sign we've messed up - we probably missed doing
    // a "set" somewhere.
    void update(const std::vector<BodyState>& state);

    void draw(Shaders::Flat3D& shader)
    {
        // shader.setColor(_color);
        // using namespace Math::Literals;
        // shader.setColor(0xff0000_rgbf);
        _mesh.draw(shader.setColor(_color));
    }

private:
    GL::Buffer _buffer;
    size_t     allocated_size = 0;
    size_t     current_size   = 0;
    GL::Mesh   _mesh;
    Color3     _color;
};
}