/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Time cursor mechanism
*/

#pragma once

#include <algorithm>

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/VertexColor.h>

namespace sim {

class TimeCursor {
public:
    double current_s = 0;
    double delta_r   = 0.001; // what fraction of the time range do we jump
    double sim_begin_s;
    double sim_progress_s;
    double sim_end_s;

    TimeCursor()
    {
        using namespace Math::Literals;

        _shader
            .setTransformationProjectionMatrix(
                Matrix3::translation((Vector2){ 0, 0 })
                * Matrix3::scaling({ 1.0, 1.0 }))
            .setColor(0xffffff_rgbf);

        _progress_mesh.setPrimitive(GL::MeshPrimitive::LineStrip)
            .setCount(2)
            .addVertexBuffer(_progress_buffer, 0, Shaders::Flat2D::Position{});

        _cursor_mesh.setPrimitive(GL::MeshPrimitive::LineStrip)
            .setCount(2)
            .addVertexBuffer(_cursor_buffer, 0, Shaders::Flat2D::Position{});
    }

    void set_color(const Color4 _color) { _shader.setColor(_color); }

    void set_range(double _t0, double _t1)
    {
        sim_begin_s = _t0;
        sim_end_s   = _t1;
        current_s   = sim_begin_s;
    }

    void forward()
    {
        current_s = std::min(
            sim_progress_s, current_s + delta_r * (sim_end_s - sim_begin_s));
    }
    void backward()
    {
        current_s = std::max(
            sim_begin_s, current_s - delta_r * (sim_end_s - sim_begin_s));
    }

    void set(double t_s)
    {
        current_s = std::min(sim_progress_s, std::max(sim_begin_s, t_s));
    }

    bool on_click(Vector2i _pos)
    {
        float width  = GL::defaultFramebuffer.viewport().sizeX();
        float height = GL::defaultFramebuffer.viewport().sizeY();

        float x = 2 * (float)_pos.x() / width - 1.0;
        float y = 1.0 - 2 * (float)_pos.y() / height;

        if ((x > left) && (x < right) && (y < top) && (y > bottom)) {
            set((sim_end_s - sim_begin_s) * (x - left) / (right - left)
                + sim_begin_s);
            return true;
        } else {
            return false;
        }
    }

    void draw()
    {
        float prog_r
            = (sim_progress_s - sim_begin_s) / (sim_end_s - sim_begin_s);
        Vector2 prog_data[]
            = { { left, mid }, { (right - left) * prog_r + left, mid } };

        _progress_buffer.setData(prog_data, GL::BufferUsage::StaticDraw);
        _progress_mesh.draw(_shader);

        float cursor_r = (current_s - sim_begin_s) / (sim_end_s - sim_begin_s);
        Vector2 cursor_data[]
            = { { (right - left) * cursor_r + left, top },
                { (right - left) * cursor_r + left, bottom } };

        _cursor_buffer.setData(cursor_data, GL::BufferUsage::StaticDraw);
        _cursor_mesh.draw(_shader);
    }

private:
    const float left   = -0.95;
    const float right  = 0.75;
    const float top    = -0.9;
    const float mid    = -0.925;
    const float bottom = -0.95;

    GL::Mesh   _progress_mesh;
    GL::Buffer _progress_buffer;

    GL::Mesh   _cursor_mesh;
    GL::Buffer _cursor_buffer;

    Shaders::Flat2D _shader;
};
}
