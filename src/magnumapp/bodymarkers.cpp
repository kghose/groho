/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This adds point markers to the location of various bodies
*/

#include "bodymarkers.hpp"
#include "applib.hpp"
#include "style.hpp"

namespace sim {

// BodyMarkers::BodyMarkers(const std::vector<Vector>& pos) { set_data(pos); }

void BodyMarkers::set_data(const RocksAndShips<SnapShot>& snapshot)
{
    size_t size = snapshot.system.size() + snapshot.fleet.size();

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

    size = 0;
    for (const auto& b : snapshot.system.bodies) {
        if (b.property.naif.is_barycenter())
            continue;
        data[size++] = v2v(b.state.pos);
    }
    for (const auto& b : snapshot.fleet.bodies) {
        data[size++] = v2v(b.state.pos);
    }

    CORRADE_INTERNAL_ASSERT_OUTPUT(buffer.unmap());

    shader.setColor(style::body_marker_color);
    mesh.setPrimitive(GL::MeshPrimitive::Points)
        .setCount(size)
        .addVertexBuffer(buffer, 0, Shaders::Flat3D::Position{});
}

void BodyMarkers::draw(const Camera& camera)
{
    shader.setTransformationProjectionMatrix(camera.get_matrix());
    mesh.draw(shader);
}
}