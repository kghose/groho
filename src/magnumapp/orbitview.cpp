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

void OrbitView::load_new_simulation_from_buffer(
    std::shared_ptr<const Buffer> buffer)
{
    buffer->lock();
    trajectories.reload_from_buffer(buffer);
    simulation_serial = buffer->simulation_serial();
    point_count       = buffer->point_count();
    buffer->release();
}

BodyTree OrbitView::get_body_tree() { return trajectories.get_body_tree(); }

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
}