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
    Font::enable_blending();

    camera.set_viewport(
        GL::defaultFramebuffer.viewport().sizeX(),
        GL::defaultFramebuffer.viewport().sizeY());

    setMinimalLoopPeriod(5);
    // setSwapInterval(1);
}

void GrohoApp::drawEvent()
{
    overlay.status = simulator.status;
    overlay.jd     = s2jd(simulator.t_s);

    // JUST TESTING - TAKE THIS OUT
    if (camera.center_id && buffer) {
        if (camera.center_id->id == 0) {
            camera.center         = { 0, 0, 0 };
            overlay.camera_center = "SSB";
        } else {
            auto idx = buffer->get_index(camera.center_id->id);
            if (idx) {
                if (buffer->get(*idx).sampled.size() > 2) {
                    auto bs       = buffer->at(*idx, camera.current_s);
                    camera.center = v2v(bs.pos);

                    overlay.camera_center
                        = buffer->metadata(*idx).property.name;
                }
            }
        }
    }

    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    //_shader.setTransformationProjectionMatrix(_projection * _transformation);
    // orrery.draw(_shader);
    orbit_view.draw(camera);
    overlay.draw(camera);

    swapBuffers();
}

void GrohoApp::viewportEvent(const Vector2i& size)
{
    GL::defaultFramebuffer.setViewport(Range2Di{ { 0, 0 }, size });
    camera.set_viewport(
        GL::defaultFramebuffer.viewport().sizeX(),
        GL::defaultFramebuffer.viewport().sizeY());

    redraw();
}

void GrohoApp::tickEvent()
{
    bool redraw_required = false;

    if (simulation_has_been_restarted()) {
        buffer = simulator.get_buffer();
        orbit_view.load_new_simulation_from_buffer(buffer);
        camera.current_s   = simulator.begin_s();
        camera.sim_begin_s = simulator.begin_s();
        camera.sim_end_s   = simulator.end_s();
        redraw_required    = true;
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

    if (event.modifiers() == InputEvent::Modifier::Alt) {
        // Scroll in time
        if (event.offset().y() > 0)
            camera.current_s
                = std::min(camera.sim_end_s, camera.current_s + camera.delta_s);
        else
            camera.current_s = std::max(
                camera.sim_begin_s, camera.current_s - camera.delta_s);

    } else {
        // Zoom in and out
        if (event.offset().y() > 0)
            camera.scale *= 1.1;
        else
            camera.scale /= 1.1;
    }

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