/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Magnum App to handle windowing and display
*/

#pragma once

#include <memory>

#include <Corrade/Containers/ArrayView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/Flat.h>

#include "camera.hpp"
#include "orbitview.hpp"
#include "overlay.hpp"
#include "simulator.hpp"

namespace sim {

using namespace Magnum;
using namespace Corrade;

class GrohoApp : public Platform::Application {
public:
    explicit GrohoApp(
        const Arguments& arguments, const sim::Simulator& simulator);

private:
    void drawEvent() override;
    void viewportEvent(const Vector2i& size) override;
    void tickEvent() override;

    bool simulation_has_been_restarted();

    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;

private:
    const sim::Simulator& simulator;

    // TODO: make this a rolling buffer for overlaying multiple sims
    std::shared_ptr<const Buffer> buffer;

    Camera    camera;
    OrbitView orbit_view;
    Overlay   overlay;

    Shaders::Flat3D _shader;

    Matrix4  _transformation, _projection;
    Vector2i _previousMousePosition;
};
}