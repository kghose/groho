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
    trajectories.draw(camera);
    // ref_plane.draw(camera);
    // sphere.draw(camera);
}

bool OrbitView::reload_from_buffer(std::shared_ptr<const Buffer> buffer)
{
    if (buffer == nullptr) {
        return false;
    }

    if ((buffer->simulation_serial() == simulation_serial)
        && buffer->point_count() == point_count) {
        return false;
    }

    if (buffer->simulation_serial() != simulation_serial) {
        time_cursor.pinned = false;
    }

    buffer->lock();

    trajectories.reload_from_buffer(buffer);

    simulation_serial = buffer->simulation_serial();
    point_count       = buffer->point_count();

    if (!time_cursor.pinned) {
        const auto& sb  = buffer->get(0);
        time_cursor.t_s = sb[sb.size() - 1].t;
    }

    buffer->release();

    return true;
}
}