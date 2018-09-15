/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing data to display for a given object
*/

#pragma once

#include <vector>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>

#include "sampledhistory.hpp"

namespace sim {

using namespace Magnum;
using namespace Corrade;

class Path {

public:
    void set_color(Color3 color) { _color = color; }

    // This copies over all the data
    template <typename T> void copy_all(const SampledHistory<T>&);

    // This copies just the new elements
    template <typename T> void copy_new(const SampledHistory<T>&);

    void set_segment(long int i0, long int i1);

    void draw(Shaders::Flat3D& shader) { _mesh.draw(shader.setColor(_color)); }

private:
    void reallocate(size_t new_size);

    enum Mode { ALL, JUST_NEW };

    template <typename T>
    void map(const SampledHistory<T>& buf_data, Mode mode);

    GL::Buffer _buffer;
    size_t     allocated_size = 0;
    size_t     current_size   = 0;

    size_t start_idx = 0;
    size_t stop_idx  = 0;

    // The simulation data buffer can carry an unsaved last point. For
    // interpolation and display purposes we want to show this, but we
    // need to also keep in mind that this point may not eventually
    // be sampled to the buffer
    bool last_point_is_provisional = false;

    GL::Mesh _mesh;
    Color3   _color;
};
}