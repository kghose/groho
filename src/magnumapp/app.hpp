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

#include "bodymarkers.hpp"
#include "camera.hpp"
#include "overlay.hpp"
#include "pathgroup.hpp"
#include "scalemodelgroup.hpp"
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

    // This (re)loads all the data, erasing any previous information and
    // recreating any visualizations and control objects
    void load_new();

    // This updates only the added new bits of the simulation
    void update_existing();

    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;

private:
    const Simulator& simulator;

    // We carry a pointer such that we can still operate on it even if the
    // simulator has moved on to a new simulation. This also keeps an avenue for
    // future expansion where we can overlay multiple simulations.
    // TODO: make this a rolling buffer for overlaying multiple sims
    std::shared_ptr<const Simulation> simulation;

    // We keep track of the last point count so we can determine if we need to
    // update our display
    size_t point_count = 0;

    struct Show {
        bool overlay      = true;
        bool trajectories = true;
        bool scale_models = true;
        bool body_markers = true;
    } show;

    // The object state at some point in time. This is used to set the markers
    RocksAndShips<SnapShot, SnapShot> snapshot;

    // Display components
    Camera          camera;
    Overlay         overlay;
    PathGroup       trajectories;
    ScaleModelGroup scale_models;
    BodyMarkers     body_markers;

    Shaders::Flat3D _shader;

    Matrix4  _transformation, _projection;
    Vector2i _previousMousePosition;
};
}