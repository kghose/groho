/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing a group of trajectories for display
*/

#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include <Magnum/Shaders/Flat.h>

#include "buffer.hpp"
#include "path.hpp"

namespace sim {

using namespace Magnum;

class PathGroup {

public:
    void reload_from_buffer(std::shared_ptr<const Buffer> buffer);
    void update(std::shared_ptr<const Buffer> buffer);
    void draw(const Matrix4& projection_matrix);

private:
    std::vector<std::shared_ptr<Path>> paths;
    Shaders::Flat3D                    _shader;
};
}