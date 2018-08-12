/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing all components of a simulation view, including
the object trajectories (paths), object markers and annotations.
*/

#include "orbitview.hpp"

namespace sim {

void OrbitView::draw(const Camera& camera)
{
    if (show_trajectories) {
        trajectories.draw(camera.get_matrix());
    }

    if (show_scale_models) {
        scale_models.draw(camera);
    }

    if (show_body_markers) {
        body_markers.draw(camera);
    }
}

// TODO: rewrite buffer so we don't have to use lock/release - pass this
// function through to buffer
void OrbitView::load_from(const Simulation& simulation)
{
    load_body_metadata(buffer);
    scale_models = ScaleModelGroup(bodies);
    body_markers = BodyMarkers(bodies);
    trajectories.reload_from_buffer(buffer);
    simulation_serial = buffer->simulation_serial();
    point_count       = buffer->point_count();
}

void OrbitView::load_body_metadata(std::shared_ptr<const Buffer> buffer)
{
    id2idx.clear();
    bodies.clear();
    for (size_t i = 0; i < buffer->body_count(); i++) {
        auto& bc = buffer->metadata(i);
        bodies.push_back(bc);
        id2idx[bc.property.code] = i;
    }
}

BodyTree OrbitView::get_body_tree()
{
    std::unordered_set<spkid_t> bodies_present;
    for (auto& b : bodies) {
        bodies_present.insert({ b.property.code, b.property.name });
    }
    return BodyTree(bodies_present);
}

bool OrbitView::buffer_has_more_points_now(std::shared_ptr<const Buffer> buffer)
{
    if (buffer == nullptr) {
        return false;
    }

    if (point_count < buffer->point_count()) {
        return true;
    }

    return false;
}

void OrbitView::update_simulation_from_buffer(
    std::shared_ptr<const Buffer> buffer)
{
    buffer->lock();
    trajectories.update(buffer);
    point_count = buffer->point_count();
    buffer->release();
}

void OrbitView::set_snapshot(
    double t_s, std::shared_ptr<const Scenario> scenario)
{
    scenario->get_snapshot_at(t_s, snapshot);
    scale_models.set_data(snapshot);
    body_markers.set_data(snapshot);
}

void OrbitView::set_camera_center_pos_from_body_state(Camera& camera)
{
    if (camera.center_id() == 0) {
        camera.set_center({ 0, 0, 0 });
    } else {
        size_t i = id2idx[camera.center_id()];
        camera.set_center(v2v(bodies[i].state.pos));
    }
}
}