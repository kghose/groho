/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing data to display for a given object
*/

#include "path.hpp"

namespace sim {

// http://doc.magnum.graphics/magnum/classMagnum_1_1GL_1_1Buffer.html#GL-Buffer-data-mapping

// This overwrites the existing data
// We don't deallocate the display buffer
void Path::set_data(const std::vector<BodyState>& state)
{
    size_t size = state.size();

    // If we try to map zero points we get a runtime error ...
    if (size == 0) {
        return;
    }

    // Only allocate if we don't have enough space already
    if (allocated_size < size) {
        while (allocated_size < size) {
            allocated_size += allocated_size + 1;
        }

        _buffer.setData(
            { nullptr, allocated_size * sizeof(Vector3) },
            GL::BufferUsage::StaticDraw);
    }

    Containers::ArrayView<Vector3> data
        = Containers::arrayCast<Vector3>(_buffer.map(
            0,
            size * sizeof(Vector3),
            GL::Buffer::MapFlag::Write
                | GL::Buffer::MapFlag::InvalidateBuffer));

    CORRADE_INTERNAL_ASSERT(data);

    for (size_t i = 0; i < size; i++) {
        data[i] = v2v(state[i].pos);
    }

    current_size = size;

    CORRADE_INTERNAL_ASSERT_OUTPUT(_buffer.unmap());

    _mesh.setPrimitive(GL::MeshPrimitive::LineStrip)
        .setCount(size)
        .addVertexBuffer(_buffer, 0, Shaders::Flat3D::Position{});
}

// This adds on the new elements of "state" to the display buffer.
// Raises runtime error if "state" is smaller than the display buffer
// because this is a sure sign we've messed up - we probably missed doing
// a "set" somewhere.
void Path::update(const std::vector<BodyState>& state)
{
    size_t size = state.size();

    if (size < current_size) {
        throw std::runtime_error("You may have forgotten to 'set' before you "
                                 "'update' the display buffer.");
    }

    if (current_size == size) {
        return;
    }

    if (allocated_size < size) {
        return set_data(state);
    }

    Containers::ArrayView<Vector3> data
        = Containers::arrayCast<Vector3>(_buffer.map(
            0,
            size * sizeof(Vector3),
            GL::Buffer::MapFlag::Write | GL::Buffer::MapFlag::FlushExplicit));

    CORRADE_INTERNAL_ASSERT(data);

    for (size_t i = current_size; i < size; i++) {
        data[i] = v2v(state[i].pos);
    }

    _buffer.flushMappedRange(
        current_size * sizeof(Vector3),
        (size - current_size) * sizeof(Vector3));

    current_size = size;

    CORRADE_INTERNAL_ASSERT_OUTPUT(_buffer.unmap());

    _mesh.setCount(current_size);
}
}