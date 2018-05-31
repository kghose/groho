/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing all components of a simulation view, including
the object trajectories (paths), object markers and annotations.
*/

#pragma once

#include "buffer.hpp"
#include "camera.hpp"
#include "pathgroup.hpp"

namespace sim {

class OrbitView {
public:
    void draw(const Camera& camera) { trajectories.draw(camera); }
    bool reload_from_buffer(std::shared_ptr<const Buffer> buffer)
    {
        if (buffer == nullptr) {
            return false;
        }

        if ((buffer->simulation_serial() == simulation_serial)
            && buffer->point_count() == point_count) {
            return false;
        }

        buffer->lock();

        trajectories.reload_from_buffer(buffer);

        simulation_serial = buffer->simulation_serial();
        point_count       = buffer->point_count();

        buffer->release();

        return true;
    }

private:
    PathGroup trajectories;

    // Metadata to figure out if we should reload a buffer
    unsigned int simulation_serial = -1;
    size_t       point_count       = 0;
};
}