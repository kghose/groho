/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Renders a text label
*/

#pragma once

#include <Corrade/PluginManager/Manager.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Complex.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>
#include <iomanip>
#include <sstream>

namespace sim {

using namespace Magnum;

class TextLabel {

public:
    TextLabel()
        : _cache(Vector2i(2048), Vector2i(512), 22)
    {
        _font = _manager.loadAndInstantiate("FreeTypeFont");
        if (!_font)
            std::exit(1);

        /* Open the font and fill glyph cache */
        Utility::Resource rs("fonts");
        if (!_font->openSingleData(rs.getRaw("Courier.ttf"), 110.0f)) {
            Error() << "Cannot open font file";
            std::exit(1);
        }

        _font->fillGlyphCache(
            _cache,
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-+,"
            ".");

        _text2.reset(new Text::Renderer2D(
            *_font, _cache, 0.035f, Text::Alignment::TopRight));
        _text2->reserve(
            40, GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);

        _text2->render("Hello!");
    }

    void draw()
    {
        _shader.setTransformationProjectionMatrix(_projection * _transformation)
            .setColor(Color3::fromHsv(216.0_degf, 0.85f, 1.0f))
            .setOutlineColor(Color3{ 0.95f })
            .setOutlineRange(0.45f, 0.35f)
            .setSmoothness(0.025f / _transformation.uniformScaling());
        _text.draw(_shader);

        _shader
            .setTransformationProjectionMatrix(
                _projection
                * Matrix3::translation(
                      1.0f / _projection.rotationScaling().diagonal()))
            .setColor(Color3{ 1.0f })
            .setOutlineRange(0.5f, 1.0f)
            .setSmoothness(0.075f);
        _text2->mesh().draw(_shader);
    }

private:
    PluginManager::Manager<Text::AbstractFont> _manager;
    std::unique_ptr<Text::AbstractFont>        _font;

    Text::DistanceFieldGlyphCache     _cache;
    GL::Mesh                          _text;
    GL::Buffer                        _vertices, _indices;
    std::unique_ptr<Text::Renderer2D> _text2;
    Shaders::DistanceFieldVector2D    _shader;

    Matrix3 _transformation;
    Matrix3 _projection;
};
}