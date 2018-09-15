/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing a group of trajectories for display
*/

#include "pathgroup.hpp"
#include "style.hpp"

namespace sim {

void PathGroup::load_from(const RocksAndShips<Record, Record>& record)
{
    paths.clear();
    for (const auto& b : record.system.bodies) {
        if (b.property.naif.is_barycenter())
            continue;

        auto p = std::shared_ptr<Path>(new Path);
        p->set_color(color);
        p->copy_all(b.history);
        paths.push_back(p);
    }

    for (const auto& b : record.fleet.bodies) {
        auto p = std::shared_ptr<Path>(new Path);
        p->set_color(color);
        p->copy_all(b.history);
        paths.push_back(p);
    }
}

void PathGroup::update_from(const RocksAndShips<Record, Record>& record)
{
    size_t j = 0;
    for (const auto& b : record.system.bodies) {
        if (b.property.naif.is_barycenter())
            continue;
        paths[j]->copy_new(b.history);
        j++;
    }
    for (const auto& b : record.fleet.bodies) {
        paths[j]->copy_new(b.history);
        j++;
    }
}

void PathGroup::set_segment(
    const RocksAndShips<Record, Record>& record, double t_s)
{
    size_t j = 0;
    for (const auto& b : record.system.bodies) {
        if (b.property.naif.is_barycenter())
            continue;

        const auto [t_idx, side] = b.history.index(t_s);
        if (side == IndexSide::INVALID) {
            continue;
        }
        paths[j]->set_segment((long int)t_idx - 10, t_idx);

        j++;
    }
    for (const auto& b : record.fleet.bodies) {

        const auto [t_idx, side] = b.history.index(t_s);
        if (side == IndexSide::INVALID) {
            continue;
        }
        paths[j]->set_segment((long int)t_idx - 10, t_idx);

        j++;
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