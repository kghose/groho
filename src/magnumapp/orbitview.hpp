/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing all components of a simulation view, including
the object trajectories (paths), object markers and annotations.
*/

#pragma once

#include "bodytree.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "pathgroup.hpp"
// #include "referenceplane.hpp"
// #include "sphere.hpp"

namespace sim {

struct TimeCursor {
    double t_s    = 0;
    bool   pinned = false;
};

class OrbitView {
public:
    void draw(const Camera& camera);

    void load_new_simulation_from_buffer(std::shared_ptr<const Buffer> buffer);
    bool buffer_has_more_points_now(std::shared_ptr<const Buffer> buffer);
    void update_simulation_from_buffer(std::shared_ptr<const Buffer> buffer);

    BodyTree body_tree;

    TimeCursor time_cursor;

private:
    PathGroup trajectories;
    // Sphere         sphere;
    // ReferencePlane ref_plane;

    // Metadata to figure out if we should reload a buffer
    // TODO: use optional
    unsigned int simulation_serial = -1;
    size_t       point_count       = 0;
};
}