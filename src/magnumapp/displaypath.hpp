/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing data to display for a given object
*/

#pragma once

#include <vector>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>

#include "vector.hpp"

namespace sim {

using namespace Magnum;
using namespace Corrade;

const float scale = 10 * 149597870.700; // 10 AU

class DisplayPath {

public:
    void set_color(Color3 color) { _color = color; }
    void set_data(const std::vector<Vector> pos)
    {
        size_t size = pos.size();
        _buffer.setData(
            { nullptr, size * sizeof(Vector3) }, GL::BufferUsage::StaticDraw);
        Containers::ArrayView<Vector3> data
            = Containers::arrayCast<Vector3>(_buffer.map(
                0,
                size * sizeof(Vector3),
                GL::Buffer::MapFlag::Write
                    | GL::Buffer::MapFlag::InvalidateBuffer));

        CORRADE_INTERNAL_ASSERT(data);

        for (size_t i = 0; i < size; i++) {
            data[i] = { static_cast<float>(pos[i].x / scale),
                        static_cast<float>(pos[i].y / scale),
                        static_cast<float>(pos[i].z / scale) };
        }

        CORRADE_INTERNAL_ASSERT_OUTPUT(_buffer.unmap());

        _mesh.setPrimitive(GL::MeshPrimitive::LineStrip)
            .setCount(size)
            .addVertexBuffer(_buffer, 0, Shaders::Flat3D::Position{});
    }

    void draw(Shaders::Flat3D& shader)
    {
        shader.setColor(_color);
        _mesh.draw(shader);
    }

private:
    GL::Buffer _buffer;
    GL::Mesh   _mesh;
    Color3     _color;
};
}