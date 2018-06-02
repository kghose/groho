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

GrohoApp::GrohoApp(const Arguments& arguments, const sim::Simulator& simulator)
    : Platform::Application{ arguments,
                             Configuration{}
                                 .setTitle("Groho orbit display")
                                 .setWindowFlags(
                                     Platform::Sdl2Application::Configuration::
                                         WindowFlag::Resizable),
                             GLConfiguration{}.setSampleCount(3) }
    , simulator(simulator)
{
    using namespace Math::Literals;

    camera.transformation
        = Matrix4::rotationX(0.0_degf) * Matrix4::rotationY(0.0_degf);

    camera.projection
        = Matrix4::perspectiveProjection(
              35.0_degf,
              Vector2{ GL::defaultFramebuffer.viewport().size() }.aspectRatio(),
              0.01f,
              100.0f)
        * Matrix4::translation(Vector3::zAxis(-10.0f));
}

void GrohoApp::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    //_shader.setTransformationProjectionMatrix(_projection * _transformation);
    // orrery.draw(_shader);
    orbit_view.draw(camera);

    swapBuffers();
}

void GrohoApp::viewportEvent(const Vector2i& size)
{
    GL::defaultFramebuffer.setViewport(Range2Di{ { 0, 0 }, size });

    camera.projection
        = Matrix4::perspectiveProjection(
              35.0_degf, (double)size.x() / (double)size.y(), 0.01f, 100.0f)
        * Matrix4::translation(Vector3::zAxis(-10.0f));

    redraw();
}

void GrohoApp::tickEvent()
{
    // TODO: make this work for multiple buffers
    if (orbit_view.reload_from_buffer(simulator.get_buffer())) {
        redraw();
    }
}

void GrohoApp::mousePressEvent(MouseEvent& event)
{
    if (event.button() != MouseEvent::Button::Left)
        return;

    _previousMousePosition = event.position();
    event.setAccepted();
}

void GrohoApp::mouseReleaseEvent(MouseEvent& event)
{
    event.setAccepted();
    redraw();
}

void GrohoApp::mouseMoveEvent(MouseMoveEvent& event)
{
    if (!(event.buttons() & MouseMoveEvent::Button::Left))
        return;

    const Vector2 delta = 3.0f
        * Vector2{ event.position() - _previousMousePosition }
        / Vector2{ GL::defaultFramebuffer.viewport().size() };

    camera.transformation = Matrix4::rotationX(Rad{ delta.y() })
        * Matrix4::rotationY(Rad{ delta.x() }) * camera.transformation;

    _previousMousePosition = event.position();
    event.setAccepted();
    redraw();
}

void GrohoApp::mouseScrollEvent(MouseScrollEvent& event)
{
    if (!event.offset().y())
        return;

    if (event.offset().y() > 0)
        camera.transformation
            = Matrix4::scaling(Vector3(1.1f)) * camera.transformation;
    else
        camera.transformation
            = Matrix4::scaling(Vector3(1.0f / 1.1f)) * camera.transformation;

    event.setAccepted();
    redraw();
}
}