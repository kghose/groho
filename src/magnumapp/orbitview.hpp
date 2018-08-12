/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing all components of a simulation view, including
the object trajectories (paths), object markers and annotations.
*/

#pragma once

#include "bodymarkers.hpp"
#include "bodytree.hpp"
#include "camera.hpp"
#include "pathgroup.hpp"
#include "scalemodelgroup.hpp"
#include "scenario.hpp"
// #include "sphere.hpp"

namespace sim {

class OrbitView {
public:
    void     draw(const Camera& camera);
    void     load_from(const Simulation&);
    BodyTree get_body_tree();
    bool     buffer_has_more_points_now(std::shared_ptr<const Buffer> buffer);
    void     update_from(const Simulation&);
    void     set_snapshot(double, std::shared_ptr<const Scenario>);
    void     set_camera_center_pos_from_body_state(Camera& camera);

public:
    bool show_trajectories = true;
    bool show_scale_models = true;
    bool show_body_markers = true;

private:
    void load_body_metadata(std::shared_ptr<const Buffer> buffer);

private:
    // The object state at some point in time. This is used to set the markers
    Objects<SnapShot> snapshot;

    // Display components
    PathGroup       trajectories;
    ScaleModelGroup scale_models;
    BodyMarkers     body_markers;

    // Metadata to figure out if we should reload a buffer
    std::optional<size_t> simulation_serial;
    std::optional<size_t> point_count;
};
}