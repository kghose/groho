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

        for (size_t i = 0; i < buffer->body_count(); i++) {

            if (buffer->metadata(i).property.body_type == BARYCENTER) {
                continue;
            }

            auto p = std::shared_ptr<Path>(new Path);
            p->set_color(Color3(buffer->metadata(i).property.color));
            p->set_data(buffer->get(i));
            paths.push_back(p);
        }
    }

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
};
}