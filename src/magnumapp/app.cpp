/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Magnum App to handle windowing and display
*/

#include "app.hpp"
#include "style.hpp"

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
    Style config_style;
    config_style.trajectory_color = { 0.2, 0.2, 0.2 };

    Font::enable_blending();

    camera.set_viewport(
        GL::defaultFramebuffer.viewport().sizeX(),
        GL::defaultFramebuffer.viewport().sizeY());

    GL::Renderer::setClearColor(config_style.canvas_color);
    time_cursor.set_color(config_style.overlay_color);
    overlay.set_color(config_style.overlay_color);
    body_markers.set_color(config_style.overlay_color);
    trajectories.set_color(config_style.trajectory_color);

    setMinimalLoopPeriod(16);
    // setSwapInterval(1);
}

void GrohoApp::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    auto center_body = body_tree.get_body();
    if (center_body.code == 0) {
        camera.set_center({ 0, 0, 0 });
    } else {
        if (center_body.is_ship()) {
            camera.set_center(v2v(snapshot.fleet[center_body].state.pos));
        } else {
            camera.set_center(v2v(snapshot.system[center_body].state.pos));
        }
    }

    body_markers.set_data(snapshot);
    scale_models.set_data(snapshot);

    overlay.status    = simulator.status;
    overlay.view_text = center_body.name + ": "
        + std::to_string(s2jd(snapshot.t_s)) + " JD: ("
        + std::to_string((float)camera.az) + ", "
        + std::to_string((float)camera.el) + ")";

    if (show.trajectories) {
        trajectories.draw(camera.get_matrix());
    }

    if (show.scale_models) {
        scale_models.draw(camera);
    }

    if (show.body_markers) {
        body_markers.draw(camera);
    }

    if (show.overlay) {
        overlay.draw(camera);
        time_cursor.draw();
    }

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

    std::shared_ptr<const Simulation> new_simulation
        = simulator.get_simulation();

    if (simulation != new_simulation) {
        // In the future we may want to keep a pair of scenarios, or a rolling
        // buffer
        simulation = new_simulation;
        load_new();
        redraw_required = true;
    } else {
        if (simulation == nullptr) {
            return;
        } else {
            if (simulation->point_count > point_count) {
                update_existing();
                redraw_required = true;
            }
        }
    }

    if (redraw_required) {
        redraw();
    }
}

void GrohoApp::load_new()
{
    time_cursor.set_range(simulation->config.begin_s, simulation->config.end_s);

    auto [record, rlock] = simulation->trajectory_data.borrow();

    trajectories.load_from(record);
    body_tree    = BodyTree(record);
    snapshot     = get_snapshot(time_cursor.current_s, record);
    scale_models = ScaleModelGroup(snapshot);

    time_cursor.sim_progress_s = record.t_s;

    point_count = simulation->point_count;
}

void GrohoApp::update_existing()
{
    auto [record, rlock] = simulation->trajectory_data.borrow();
    trajectories.update_from(record);
    time_cursor.sim_progress_s = record.t_s;

    point_count = simulation->point_count;
}

void GrohoApp::mousePressEvent(MouseEvent& event)
{
    if (event.button() != MouseEvent::Button::Left)
        return;

    if (time_cursor.on_click(event.position())) {
        event.setAccepted();
        synchronize_snapshot();
        return;
    }

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
            time_cursor.forward();
        } else {
            time_cursor.backward();
        }
        synchronize_snapshot();
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
    case KeyEvent::Key::O:
        show.overlay ^= true;
        break;
    case KeyEvent::Key::P:
        show.trajectories ^= true;
        break;
    case KeyEvent::Key::M:
        show.body_markers ^= true;
        break;
    case KeyEvent::Key::B:
        show.scale_models ^= true;
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
        body_tree.change_item(BodyTree::PREVBODY);
        break;
    case KeyEvent::Key::Right:
        body_tree.change_item(BodyTree::NEXTBODY);
        break;
    case KeyEvent::Key::Up:
        body_tree.change_cat(BodyTree::PREVCAT);
        break;
    case KeyEvent::Key::Down:
        body_tree.change_cat(BodyTree::NEXTCAT);
        break;
    default:
        break;
    }

    redraw();
}

void GrohoApp::synchronize_snapshot()
{
    auto [record, rlock] = simulation->trajectory_data.borrow();
    snapshot             = get_snapshot(time_cursor.current_s, record);
    trajectories.set_segment(record, time_cursor.current_s);
}
}