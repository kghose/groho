/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This adds point markers to the location of various bodies
*/

#include "bodymarkers.hpp"
#include "applib.hpp"
#include "style.hpp"

namespace sim {

BodyMarkers::BodyMarkers(const std::vector<Body>& bodies) { set_data(bodies); }

void BodyMarkers::set_data(const std::vector<Body>& bodies)
{
    size_t size = bodies.size();

    // If we try to map zero points we get a runtime error ...
    if (size == 0) {
        return;
    }

    buffer.setData(
        { nullptr, size * sizeof(Vector3) }, GL::BufferUsage::StaticDraw);
    Containers::ArrayView<Vector3> data
        = Containers::arrayCast<Vector3>(buffer.map(
            0,
            size * sizeof(Vector3),
            GL::Buffer::MapFlag::Write
                | GL::Buffer::MapFlag::InvalidateBuffer));

    CORRADE_INTERNAL_ASSERT(data);

    size_t j = 0;
    for (size_t i = 0; i < size; i++) {
        if (bodies[i].property.body_type != BARYCENTER) {
            data[j++] = v2v(bodies[i].state.pos);
        }
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(buffer.unmap());

    shader.setColor(style::body_marker_color);
    mesh.setPrimitive(GL::MeshPrimitive::Points)
        .setCount(j)
        .addVertexBuffer(buffer, 0, Shaders::Flat3D::Position{});
}

void BodyMarkers::draw(const Camera& camera)
{
    shader.setTransformationProjectionMatrix(camera.get_matrix());
    mesh.draw(shader);
}
}