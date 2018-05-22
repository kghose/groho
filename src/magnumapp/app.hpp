/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Magnum App to handle windowing and display
*/

#pragma once

#include <Corrade/Containers/ArrayView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFrameBuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/Flat.h>

#include "simulator.hpp"

using namespace Magnum;
using namespace Corrade;

const float scale = 10 * 149597870.700; // 10 AU

struct Path {
    GL::Buffer _buffer;
    GL::Mesh   _mesh;
    Color3     _color;

    void set_data(const std::vector<sim::Vector> pos)
    {
        size_t size = pos.size();
        _buffer.setData(
            { nullptr, size * sizeof(Vector3) }, GL::BufferUsage::StaticDraw);
        Containers::ArrayView<Vector3> data
            = Containers::arrayCast<Vector3>(_buffer.map(
                0,
                size * sizeof(Vector3),
                GL::Buffer::MapFlag::Write
                    | GL::Buffer::MapFlag::InvalidateBuffer));
        CORRADE_INTERNAL_ASSERT(data);
        float max_x = 0, max_y = 0, max_z = 0;
        for (size_t i = 0; i < size; i++) {
            data[i] = { static_cast<float>(pos[i].x / scale),
                        static_cast<float>(pos[i].y / scale),
                        static_cast<float>(pos[i].z / scale) };

            max_x = data[i].x() > max_x ? data[i].x() : max_x;
            max_x = data[i].y() > max_y ? data[i].y() : max_y;
            max_x = data[i].z() > max_z ? data[i].z() : max_z;

            // data[i] = { static_cast<float>(i) / static_cast<float>(size),
            //             static_cast<float>(i) / static_cast<float>(size),
            //             static_cast<float>(i) / static_cast<float>(size) };
        }

        std::cout << max_x << ", " << max_y << ", " << max_z << std::endl;
        CORRADE_INTERNAL_ASSERT_OUTPUT(_buffer.unmap());
        // _buffer.setData(data, GL::BufferUsage::StaticDraw);

        _mesh.setPrimitive(GL::MeshPrimitive::LineStrip)
            .setCount(size)
            .addVertexBuffer(_buffer, 0, Shaders::Flat3D::Position{});
    }
};

class GrohoApp : public Platform::Application {
public:
    explicit GrohoApp(
        const Arguments& arguments, const sim::Simulator& simulator);

private:
    void drawEvent() override;

    void set_buffers();

    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    const sim::Simulator& simulator;

    std::shared_ptr<const sim::Buffer> buffer;

    Shaders::Flat3D _shader;

    Matrix4  _transformation, _projection;
    Vector2i _previousMousePosition;
};

GrohoApp::GrohoApp(const Arguments& arguments, const sim::Simulator& simulator)
    : Platform::Application{ arguments,
                             Configuration{}
                                 .setTitle("Groho orbit display")
                                 .setWindowFlags(
                                     Platform::Sdl2Application::Configuration::
                                         WindowFlag::Resizable) }
    , simulator(simulator)
{
    using namespace Math::Literals;

    // const Vector3 data[]{ { { -0.5f, -0.5f, 0.0f } },
    //                       { { 0.5f, -0.5f, 0.0f } },
    //                       { { 0.0f, 0.5f, 0.0f } },
    //                       { { -0.5f, -0.5f, 0.0f } } };

    // _buffer.setData(data, GL::BufferUsage::StaticDraw);
    // _mesh.setPrimitive(GL::MeshPrimitive::LineStrip)
    //     .setCount(4)
    //     .addVertexBuffer(_buffer, 0, Shaders::Flat3D::Position{});

    // _transformation = Matrix4::translation(Vector3::zAxis(-0.5f));
    // _projection
    //     = Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.001f, 100.0f);

    _transformation
        = Matrix4::rotationX(30.0_degf) * Matrix4::rotationY(40.0_degf);

    //_color = Color3::fromHsv(35.0_degf, 1.0f, 1.0f);

    _projection
        = Matrix4::perspectiveProjection(
              35.0_degf,
              Vector2{ GL::defaultFramebuffer.viewport().size() }.aspectRatio(),
              0.01f,
              100.0f)
        * Matrix4::translation(Vector3::zAxis(-10.0f));

    _shader.setColor(0x2f83cc_rgbf);
}

void GrohoApp::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    _shader.setTransformationProjectionMatrix(_projection * _transformation);

    // TODO: make this work for multiple buffers
    buffer = simulator.get_buffer();

    if (buffer != nullptr) {
        buffer->lock();
        std::cout << "Boo!" << std::endl;
        for (size_t i = 0; i < buffer->body_count(); i++) {
            if (buffer->metadata(i).spkid < 301)
                continue;
            if (buffer->metadata(i).spkid > 399)
                continue;

            Path p;
            p.set_data(buffer->get(i));
            p._mesh.draw(_shader);
        }
        buffer->release();
    }

    swapBuffers();
}

void GrohoApp::mousePressEvent(MouseEvent& event)
{
    if (event.button() != MouseEvent::Button::Left)
        return;

    _previousMousePosition = event.position();
    event.setAccepted();
}

void GrohoApp::mouseReleaseEvent(MouseEvent& event)
{
    //_color = Color3::fromHsv(_color.hue() + 50.0_degf, 1.0f, 1.0f);

    event.setAccepted();
    redraw();
}

void GrohoApp::mouseMoveEvent(MouseMoveEvent& event)
{
    if (!(event.buttons() & MouseMoveEvent::Button::Left))
        return;

    const Vector2 delta = 3.0f
        * Vector2{ event.position() - _previousMousePosition }
        / Vector2{ GL::defaultFramebuffer.viewport().size() };

    _transformation = Matrix4::rotationX(Rad{ delta.y() })
        * Matrix4::rotationY(Rad{ delta.x() }) * _transformation;

    _previousMousePosition = event.position();
    event.setAccepted();
    redraw();
}

void GrohoApp::mouseScrollEvent(MouseScrollEvent& event)
{
    if (!event.offset().y())
        return;

    if (event.offset().y() > 0)
        _transformation = Matrix4::scaling(Vector3(1.1f)) * _transformation;
    else
        _transformation
            = Matrix4::scaling(Vector3(1.0f / 1.1f)) * _transformation;

    event.setAccepted();
    redraw();
}