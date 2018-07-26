/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This adds triangle markers to the location of various bodies
*/

#pragma once

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Primitives/Circle.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData3D.h>

#include "body.hpp"
#include "camera.hpp"
#include "vector.hpp"
#include "vector2vector.hpp"

namespace sim {

using namespace Magnum;
using namespace Corrade;

class BodyMarker {
public:
    BodyMarker(double r, bool draw_scaled = true, Color3 c = Color3{ 1, 1, 1 })
        : color(c)
        , scaling(Matrix4::scaling(
              draw_scaled ? Vector3(r / globals::scale) : Vector3(0.02)))
    {
        const Trade::MeshData3D circle = Primitives::circle3DWireframe(15);

        vertexBuffer.setData(circle.positions(0), GL::BufferUsage::StaticDraw);

        shader.setColor(color);
        mesh.setPrimitive(circle.primitive())
            .setCount(15)
            .addVertexBuffer(vertexBuffer, 0, Shaders::Flat3D::Position{});
    }

    void set_pos(const Vector3& p) { pos = p; }

    void draw_scaled(const Camera& camera)
    {
        shader.setTransformationProjectionMatrix(
            camera.get_billboard_matrix(pos) * scaling);
        mesh.draw(shader);
    }

    void draw_unscaled(const Camera& camera)
    {
        shader.setTransformationProjectionMatrix(
            camera.get_unscaled_billboard_matrix(pos) * scaling);
        mesh.draw(shader);
    }

private:
    GL::Buffer      vertexBuffer;
    GL::Mesh        mesh;
    Shaders::Flat3D shader;

    Vector3 pos;
    // const float  radius;
    const Color3 color;
    // const bool    draw_scaled;
    const Matrix4 scaling;
};

class BodyMarkers {
public:
    BodyMarkers() = default;
    void clear() { markers.clear(); }
    void add(const Body& body, bool draw_scaled = true)
    {
        markers.push_back(
            { body.property.r, draw_scaled, Color3(body.property.color) });
    }
    virtual void draw(const std::vector<Body>& bodies, const Camera& camera)
        = 0;

protected:
    std::vector<BodyMarker> markers;
};

class ScaledBodyMarkers : public BodyMarkers {
public:
    void draw(const std::vector<Body>& bodies, const Camera& camera)
    {
        for (size_t i = 0; i < markers.size(); i++) {
            if (bodies[i].property.body_type != BARYCENTER) {
                markers[i].set_pos(v2v(bodies[i].state.pos));
                markers[i].draw_scaled(camera);
            }
        }
    }
};

class UnscaledBodyMarkers : public BodyMarkers {
public:
    void draw(const std::vector<Body>& bodies, const Camera& camera)
    {
        for (size_t i = 0; i < markers.size(); i++) {
            if (bodies[i].property.body_type != BARYCENTER) {
                markers[i].set_pos(v2v(bodies[i].state.pos));
                markers[i].draw_unscaled(camera);
            }
        }
    }
};
}