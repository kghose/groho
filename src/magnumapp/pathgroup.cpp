/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing a group of trajectories for display
*/

#include "pathgroup.hpp"
#include "style.hpp"

namespace sim {

void PathGroup::reload_from_buffer(std::shared_ptr<const Buffer> buffer)
{
    paths.clear();
    for (size_t i = 0; i < buffer->body_count(); i++) {
        auto p = std::shared_ptr<Path>(new Path);
        // p->set_color(Color3(buffer->metadata(i).property.color));
        p->set_color(style::trajectory_color);
        if (buffer->metadata(i).property.body_type != BARYCENTER) {
            p->copy_all(buffer->get(i));
        }
        paths.push_back(p);
    }
}

void PathGroup::update(std::shared_ptr<const Buffer> buffer)
{
    for (size_t i = 0; i < buffer->body_count(); i++) {
        if (buffer->metadata(i).property.body_type != BARYCENTER) {
            paths[i]->copy_new(buffer->get(i));
        }
    }
}

void PathGroup::draw(const Matrix4& projection_matrix)
{
    _shader.setTransformationProjectionMatrix(projection_matrix);
    for (auto& p : paths) {
        p->draw(_shader);
    }
}
}