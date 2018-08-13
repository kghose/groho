/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Magnum App to handle windowing and display
*/

#include "app.hpp"

namespace sim {

GrohoApp::GrohoApp(const Arguments& arguments, const sim::Simulator& simulator)
    : Platform::Application{ arguments,
                             Configuration{}
                                 .setTitle("Groho orbit display")
                                 .setWindowFlags(
                                     Platform::Sdl2Application::Configuration::
                                         WindowFlag::Resizable),
                             GLConfiguration{}.setSampleCount(4) }
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
    overlay.jd     = s2jd(simulator.t_s());

    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

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

    std::shared_ptr<const Scenario> new_scenario = simulator.get_scenario();

    if (scenario != new_scenario) {
        scenario = new_scenario;
        reload();
        redraw_required = true;
    } else {
        if (scenario == nullptr)
            return;
    }

    if (simulation_has_been_restarted()) {
        // scenario = simulator.get_scenario();
        camera.set_body_tree(orbit_view.get_body_tree());
        camera.set_time_range(simulator.begin_s(), simulator.end_s());
        orbit_view.set_body_state_at_time_cursor(camera, buffer);
        orbit_view.set_camera_center_pos_from_body_state(camera);
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

void GrohoApp::reload()
{
    scenario->read(
        {},
        std::bind(&OrbitView::load_from, &orbit_view, std::placeholders::_1));

    camera.set_body_tree(orbit_view.get_body_tree());
    camera.set_time_range(simulator.begin_s(), simulator.end_s());

    orbit_view
        .

        orbit_view.set_body_state_at_time_cursor(camera, buffer);
    orbit_view.set_camera_center_pos_from_body_state(camera);
}

// The simulator has been restarted with a new simulation
bool GrohoApp::simulation_has_been_restarted()
{
    std::shared_ptr<const Scenario> new_scenario = simulator.get_scenario();

    // We can probably achieve this whole effect by comparing pointers
    // but this code shows our real intent explicitly

    if (new_scenario == nullptr) {
        return false;
    }

    if ((scenario != nullptr)
        && (scenario->simulation_serial()
            == new_scenario->simulation_serial())) {
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

    camera.increase_az_by(Deg{ Rad{ delta.x() } });
    camera.increase_el_by(Deg{ Rad{ delta.y() } });

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
        if (event.offset().y() > 0) {
            camera.step_forward_in_time();
        } else {
            camera.step_backward_in_time();
        }
        orbit_view.set_body_state_at_time_cursor(camera, buffer);
        orbit_view.set_camera_center_pos_from_body_state(camera);
    } else {
        // Zoom in and out
        if (event.offset().y() > 0)
            camera.zoom_in();
        else
            camera.zoom_out();
    }

    event.setAccepted();
    redraw();
}

void GrohoApp::keyPressEvent(KeyEvent& event)
{
    switch (event.key()) {
    case KeyEvent::Key::P:
        orbit_view.show_trajectories ^= true;
        break;
    case KeyEvent::Key::M:
        orbit_view.show_body_markers ^= true;
        break;
    case KeyEvent::Key::B:
        orbit_view.show_scale_models ^= true;
        break;
    default:
        break;
    }

    redraw();
}

void GrohoApp::keyReleaseEvent(KeyEvent& event)
{
    switch (event.key()) {
    case KeyEvent::Key::Left:
        camera.prev_body();
        break;
    case KeyEvent::Key::Right:
        camera.next_body();
        break;
    case KeyEvent::Key::Up:
        camera.prev_category();
        break;
    case KeyEvent::Key::Down:
        camera.next_category();
        break;
    default:
        break;
    }

    orbit_view.set_camera_center_pos_from_body_state(camera);
    redraw();
}
}