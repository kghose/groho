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
    ScaleModel(const Body& body)
        : body_type(body.property.body_type)
        , color(from_rgb(body.property.color))
        , scaling(Matrix4::scaling(Vector3(body.property.r / style::scale)))
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
        shader.setTransformationProjectionMatrix(
            camera.get_billboard_matrix(pos) * scaling);
        mesh.draw(shader);
    }

public:
    const BodyType body_type;

private:
    GL::Buffer      vertexBuffer;
    GL::Mesh        mesh;
    Shaders::Flat3D shader;

    Vector3       pos;
    const Color3  color;
    const Matrix4 scaling;
};

class ScaleModelGroup {
public:
    ScaleModelGroup() = default;
    ScaleModelGroup(const std::vector<Body>& bodies)
    {
        for (auto& b : bodies) {
            models.push_back({ b });
        }
    }

    void set_data(const std::vector<Body>& bodies)
    {
        for (size_t i = 0; i < models.size(); i++) {
            if (bodies[i].property.body_type != BARYCENTER) {
                models[i].set_pos(v2v(bodies[i].state.pos));
            }
        }
    }

    void draw(const Camera& camera)
    {
        for (auto& m : models) {
            if (m.body_type != BARYCENTER) {
                m.draw(camera);
            }
        }
    }

private:
    std::vector<ScaleModel> models;
};
}