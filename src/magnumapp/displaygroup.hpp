/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing a group of data to display for a simulation
*/

#pragma once

#include <vector>

#include <Magnum/Shaders/Flat.h>

#include "buffer.hpp"
#include "displaypath.hpp"

namespace sim {

using namespace Magnum;

class DisplayGroup {

public:
    // return true if a reload ocurred
    bool reload_from_buffer(std::shared_ptr<const Buffer> buffer)
    {
        if (buffer == nullptr) {
            return false;
        }

        if ((buffer->simulation_serial() == simulation_serial)
            && buffer->point_count() == point_count) {
            return false;
        }

        LOG_S(INFO) << "Reloading buffer";

        paths.clear();

        buffer->lock();

        for (size_t i = 0; i < buffer->body_count(); i++) {

            if (!buffer->metadata(i).real_body) {
                continue;
            }

            auto p = std::shared_ptr<DisplayPath>(new DisplayPath);
            p->set_color(Color3(buffer->metadata(i).color));
            p->set_data(buffer->get(i));
            paths.push_back(p);
        }

        simulation_serial = buffer->simulation_serial();
        point_count       = buffer->point_count();

        buffer->release();

        return true;
    }

    void draw(Shaders::Flat3D& shader)
    {
        for (auto& p : paths) {
            p->draw(shader);
        }
    }

private:
    std::vector<std::shared_ptr<DisplayPath>> paths;

    // Metadata to figure out if we should reload a buffer
    unsigned int simulation_serial = -1;
    size_t       point_count       = 0;
};
}