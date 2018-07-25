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

class OrbitView {
public:
    void draw(const Camera& camera);
    void load_new_simulation_from_buffer(std::shared_ptr<const Buffer> buffer);
    BodyTree get_body_tree();
    bool     buffer_has_more_points_now(std::shared_ptr<const Buffer> buffer);
    void update_simulation_from_buffer(std::shared_ptr<const Buffer> buffer);
    void set_body_state_at_time_cursor(
        const Camera& camera, std::shared_ptr<const Buffer> buffer);
    void set_camera_center_pos_from_body_state(Camera& camera);

private:
    void load_body_metadata(std::shared_ptr<const Buffer> buffer);

private:
    // Maps a NAIF code to an index for quickly finding a particular
    // bodyconstant or state
    std::unordered_map<int, size_t> id2idx;

    // This is just useful to have. For example, we generate the body tree from
    // this
    std::vector<BodyConstant> body;

    // We update this whenever our time cursor moves
    std::vector<BodyState> body_state_at_time_cursor;

    PathGroup trajectories;

    // Sphere         sphere;
    // ReferencePlane ref_plane;

    // Metadata to figure out if we should reload a buffer
    // TODO: use optional
    unsigned int simulation_serial = -1;
    size_t       point_count       = 0;
};
}