/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing data to display for a given object
*/

#include "path.hpp"

namespace sim {

// This copies over all the data
void Path::copy_all(Buffer::data_t buf_data)
{
    if (buf_data.required_size() > 0) {
        map(buf_data, ALL);
        _mesh.setPrimitive(GL::MeshPrimitive::LineStrip)
            .setCount(current_size)
            .addVertexBuffer(_buffer, 0, Shaders::Flat3D::Position{});
    }
}

// This copies just the new elements
void Path::copy_new(Buffer::data_t buf_data)
{
    if (buf_data.required_size() > 0) {
        size_t new_size = buf_data.required_size();
        if (allocated_size < new_size) {
            return copy_all(buf_data);
        }
        map(buf_data, JUST_NEW);
        _mesh.setCount(current_size);
    }
}

void Path::reallocate(size_t new_size)
{
    while (allocated_size < new_size) {
        allocated_size += allocated_size + 1;
    }

    _buffer.setData(
        { nullptr, allocated_size * sizeof(Vector3) },
        GL::BufferUsage::StaticDraw);
}

void Path::map(Buffer::data_t buf_data, Mode mode)
{
    size_t new_size = buf_data.required_size();

    size_t offset = 0;
    if (mode == JUST_NEW) {
        offset = current_size;
        if (last_point_is_provisional) {
            offset -= 1;
        }
    } else {
        reallocate(new_size);
    }

    // http://doc.magnum.graphics/magnum/classMagnum_1_1GL_1_1Buffer.html#GL-Buffer-data-mapping
    Containers::ArrayView<Vector3> data
        = Containers::arrayCast<Vector3>(_buffer.map(
            offset * sizeof(Vector3),
            (new_size - offset) * sizeof(Vector3),
            GL::Buffer::MapFlag::Write | GL::Buffer::MapFlag::FlushExplicit));

    CORRADE_INTERNAL_ASSERT(data);

    size_t i = offset;
    for (; i < buf_data.sampled.size(); i++) {
        data[i - offset] = v2v(buf_data.sampled[i].pos);
    }
    if (buf_data.unsampled) {
        data[i - offset]          = v2v(buf_data.unsampled->pos);
        last_point_is_provisional = true;
    } else {
        last_point_is_provisional = false;
    }

    if (mode == JUST_NEW) {
        _buffer.flushMappedRange(0, (new_size - offset) * sizeof(Vector3));
    }

    current_size = new_size;

    CORRADE_INTERNAL_ASSERT_OUTPUT(_buffer.unmap());
}
}