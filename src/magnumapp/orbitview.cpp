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
    body.clear();
    for (size_t i = 0; i < buffer->body_count(); i++) {
        body.push_back(buffer->metadata(i).property);
    }
}

BodyTree OrbitView::get_body_tree()
{
    std::unordered_set<spkid_t> bodies_present;
    for (auto& b : body) {
        bodies_present.insert({ b.code, b.name });
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
}