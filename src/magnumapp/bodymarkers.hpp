/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This adds point markers to the location of various bodies
*/

#pragma once

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData3D.h>

#include "body.hpp"
#include "camera.hpp"

namespace sim {

using namespace Magnum;

class BodyMarkers {
public:
    BodyMarkers() = default;
    BodyMarkers(const std::vector<Body>& bodies);
    void set_data(const std::vector<Body>& bodies);
    void draw(const Camera& camera);

private:
    GL::Buffer      buffer;
    GL::Mesh        mesh;
    Color3          color;
    Shaders::Flat3D shader;
};
}