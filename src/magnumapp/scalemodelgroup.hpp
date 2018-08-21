/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This draws scalled, billboard circles to mimic the planetary bodies
*/

#pragma once

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Primitives/Circle.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData3D.h>

#include "applib.hpp"
#include "body.hpp"
#include "camera.hpp"
#include "style.hpp"
#include "vector.hpp"

namespace sim {

using namespace Magnum;
using namespace Corrade;

// A billboard circle that fakes a body
class ScaleModel {
public:
    ScaleModel(unsigned int _color, float r, bool _vis)
        : color(from_rgb(_color))
        , scaling(Matrix4::scaling(Vector3(r / style::scale)))
        , visible(_vis)
    {
        const Trade::MeshData3D circle = Primitives::circle3DWireframe(15);
        // const Trade::MeshData3D circle = Primitives::circle3DSolid(15);

        vertexBuffer.setData(circle.positions(0), GL::BufferUsage::StaticDraw);

        shader.setColor(color);
        mesh.setPrimitive(circle.primitive())
            .setCount(15)
            .addVertexBuffer(vertexBuffer, 0, Shaders::Flat3D::Position{});
    }

    void set_pos(const Vector3& p) { pos = p; }

    void draw(const Camera& camera)
    {
        if (visible) {
            shader.setTransformationProjectionMatrix(
                camera.get_billboard_matrix(pos) * scaling);
            mesh.draw(shader);
        }
    }

private:
    GL::Buffer      vertexBuffer;
    GL::Mesh        mesh;
    Shaders::Flat3D shader;

    Vector3       pos;
    const Color3  color;
    const Matrix4 scaling;

    const bool visible = false;
};

class ScaleModelGroup {
public:
    ScaleModelGroup() = default;
    ScaleModelGroup(const RocksAndShips<SnapShot, SnapShot>& snap_shot)
    {
        for (const auto& b : snap_shot.system.bodies) {
            models.push_back({ b.property.color,
                               b.property.r,
                               !b.property.naif.is_barycenter() });
        }
    }

    void set_data(const RocksAndShips<SnapShot, SnapShot>& snapshot)
    {
        for (size_t i = 0; i < models.size(); i++) {
            models[i].set_pos(v2v(snapshot.system.bodies[i].state.pos));
        }
    }

    void draw(const Camera& camera)
    {
        for (auto& m : models) {
            m.draw(camera);
        }
    }

private:
    std::vector<ScaleModel> models;
};
}