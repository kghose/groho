/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Magnum App to handle windowing and display
*/

#include "app.hpp"
#include "vector2vector.hpp"

namespace sim {

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

    camera.aspect_ratio = (float)GL::defaultFramebuffer.viewport().sizeX()
        / GL::defaultFramebuffer.viewport().sizeY();
}

void GrohoApp::drawEvent()
{
    // JUST TESTING - TAKE THIS OUT
    if (camera.center_id && buffer) {
        auto idx = buffer->get_index(camera.center_id->id);
        if (idx) {
            if (buffer->get(*idx).sampled.size() > 2) {
                auto bs       = buffer->at(*idx, simulator.t_s);
                camera.center = v2v(bs.pos);
            }
        }
    }

    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    //_shader.setTransformationProjectionMatrix(_projection * _transformation);
    // orrery.draw(_shader);
    orbit_view.draw(camera);

    swapBuffers();
}

void GrohoApp::viewportEvent(const Vector2i& size)
{
    GL::defaultFramebuffer.setViewport(Range2Di{ { 0, 0 }, size });

    camera.aspect_ratio = (double)size.x() / (double)size.y();
    redraw();
}

void GrohoApp::tickEvent()
{
    bool redraw_required = false;

    if (simulation_has_been_restarted()) {
        buffer = simulator.get_buffer();
        orbit_view.load_new_simulation_from_buffer(buffer);
        redraw_required = true;
    }

    if (orbit_view.buffer_has_more_points_now(buffer)) {
        orbit_view.update_simulation_from_buffer(buffer);
        redraw_required = true;
    }

    if (redraw_required) {
        redraw();
    }
}

// The simulator has been restarted with a new simulation
bool GrohoApp::simulation_has_been_restarted()
{
    std::shared_ptr<const Buffer> sim_buffer = simulator.get_buffer();

    // We can probably achieve this whole effect by comparing pointers
    // but this code shows our real intent explicitly

    if (sim_buffer == nullptr) {
        return false;
    }

    if ((buffer != nullptr)
        && (buffer->simulation_serial() == sim_buffer->simulation_serial())) {
        return false;
    }

    return true;
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

    camera.az += Deg{ Rad{ delta.x() } };
    camera.el += Deg{ Rad{ delta.y() } };

    _previousMousePosition = event.position();
    event.setAccepted();
    redraw();
}

void GrohoApp::mouseScrollEvent(MouseScrollEvent& event)
{
    if (!event.offset().y())
        return;

    if (event.offset().y() > 0)
        camera.scale *= 1.1;
    else
        camera.scale /= 1.1;

    event.setAccepted();
    redraw();
}

void GrohoApp::keyReleaseEvent(KeyEvent& event)
{
    switch (event.key()) {
    case KeyEvent::Key::Left:
        camera.center_id = orbit_view.body_tree.change_item(BodyTree::PREVBODY);
        break;
    case KeyEvent::Key::Right:
        camera.center_id = orbit_view.body_tree.change_item(BodyTree::NEXTBODY);
        break;
    case KeyEvent::Key::Up:
        camera.center_id = orbit_view.body_tree.change_cat(BodyTree::PREVCAT);
        break;
    case KeyEvent::Key::Down:
        camera.center_id = orbit_view.body_tree.change_cat(BodyTree::NEXTCAT);
        break;
    default:
        break;
    }
    redraw();
}
}