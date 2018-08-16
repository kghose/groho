/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017, 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient container for managing data to display for a given object
*/

#include "path.hpp"
#include "applib.hpp"

namespace sim {

// This copies over all the data
template <typename T> void Path::copy_all(const SubBuffer<T>& buf_data)
{
    if (buf_data.effective_size() > 0) {
        map(buf_data, ALL);
        _mesh.setPrimitive(GL::MeshPrimitive::LineStrip)
            .setCount(current_size)
            .addVertexBuffer(_buffer, 0, Shaders::Flat3D::Position{});
    }
}

// This copies just the new elements
template <typename T> void Path::copy_new(const SubBuffer<T>& buf_data)
{
    if (buf_data.effective_size() > 0) {
        size_t new_size = buf_data.effective_size();
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

template <typename T> void Path::map(const SubBuffer<T>& buf_data, Mode mode)
{
    size_t new_size = buf_data.effective_size();

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

    size_t      i  = offset;
    const auto& _p = buf_data.sampled();
    for (; i < _p.size(); i++) {
        data[i - offset] = v2v(_p[i].pos);
    }
    if (buf_data.unsampled()) {
        data[i - offset]          = v2v(buf_data.unsampled()->pos);
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

template void
Path::copy_all<RockLike::State>(const SubBuffer<RockLike::State>& buf_data);

template void
Path::copy_new<RockLike::State>(const SubBuffer<RockLike::State>& buf_data);

template void Path::map<RockLike::State>(
    const SubBuffer<RockLike::State>& buf_data, Mode mode);

template void
Path::copy_all<ShipLike::State>(const SubBuffer<ShipLike::State>& buf_data);

template void
Path::copy_new<ShipLike::State>(const SubBuffer<ShipLike::State>& buf_data);

template void Path::map<ShipLike::State>(
    const SubBuffer<ShipLike::State>& buf_data, Mode mode);
}