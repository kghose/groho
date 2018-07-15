/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing a group of data to display for a simulation
*/

#pragma once

#include <memory>
#include <vector>

#include <Magnum/Shaders/Flat.h>

#include "buffer.hpp"
#include "camera.hpp"
#include "path.hpp"

namespace sim {

using namespace Magnum;

class PathGroup {

public:
    void reload_from_buffer(std::shared_ptr<const Buffer> buffer)
    {
        paths.clear();
        body_tree = BodyTree();

        for (size_t i = 0; i < buffer->body_count(); i++) {

            if (buffer->metadata(i).property.body_type == BARYCENTER) {
                continue;
            }

            auto p = std::shared_ptr<Path>(new Path);
            p->set_color(Color3(buffer->metadata(i).property.color));
            p->copy_all(buffer->get(i));
            paths.push_back(p);

            body_tree.add(spkid_t(buffer->metadata(i).property.code));
        }
    }

    void update(std::shared_ptr<const Buffer> buffer)
    {
        for (size_t i = 0, j = 0; i < buffer->body_count(); i++) {

            if (buffer->metadata(i).property.body_type == BARYCENTER) {
                continue;
            }

            paths[j++]->copy_new(buffer->get(i));
        }
    }

    BodyTree get_body_tree() { return body_tree; }

    void draw(const Camera& camera)
    {
        _shader.setTransformationProjectionMatrix(camera.get_matrix());
        for (auto& p : paths) {
            p->draw(_shader);
        }
    }

private:
    std::vector<std::shared_ptr<Path>> paths;
    Shaders::Flat3D                    _shader;

    BodyTree body_tree;
};
}