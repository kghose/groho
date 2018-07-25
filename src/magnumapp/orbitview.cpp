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
    trajectories.draw(camera.get_matrix());
    // ref_plane.draw(camera);
    // sphere.draw(camera);
}

// TODO: rewrite buffer so we don't have to use lock/release - pass this
// function through to buffer
void OrbitView::load_new_simulation_from_buffer(
    std::shared_ptr<const Buffer> buffer)
{
    buffer->lock();
    trajectories.reload_from_buffer(buffer);
    simulation_serial = buffer->simulation_serial();
    point_count       = buffer->point_count();
    load_body_metadata(buffer);
    buffer->release();
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

void OrbitView::set_body_state_at_time_cursor(
    const Camera& camera, std::shared_ptr<const Buffer> buffer)
{
    if (buffer == nullptr)
        return;
    for (size_t i = 0; i < bodies.size(); i++) {
        if ((bodies[i].property.body_type != BARYCENTER)
            && (buffer->get(i).sampled.size() > 0)) {
            bodies[i].state = buffer->at(i, camera.current_s());
            // TODO: Improve how we get sampled data (interpolation project)
            // Let's try an over all higher sampling rate. This is lowest
            // complexity.
            // Also, we should improve the buffer interface for getting back
            // data
            // 1. "at" should use spline interpolation
            // 2. "at" should have a companion function that wraps
            // buffer->get(i).sampled.size()
        }
    }
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