/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Magnum App to handle windowing and display
*/

#pragma once

#include <Corrade/Containers/ArrayView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFrameBuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/Flat.h>

#include "orbitview.hpp"
#include "simulator.hpp"

using namespace Magnum;
using namespace Corrade;

namespace sim {

class GrohoApp : public Platform::Application {
public:
    explicit GrohoApp(
        const Arguments& arguments, const sim::Simulator& simulator);

private:
    void drawEvent() override;
    void viewportEvent(const Vector2i& size) override;
    void tickEvent() override;

    void set_buffers();

    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    const sim::Simulator& simulator;

    Camera    camera;
    OrbitView orbit_view;

    Shaders::Flat3D _shader;

    Matrix4  _transformation, _projection;
    Vector2i _previousMousePosition;
};

}