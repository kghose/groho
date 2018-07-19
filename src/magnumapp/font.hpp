/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Handles font cache
*/

#pragma once

#include <Corrade/PluginManager/Manager.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>

namespace sim {

using namespace Magnum;

class Font {

public:
    Font(std::string fname = "resources/UbuntuMono-R.ttf")
        : _cache(Vector2i(2048), Vector2i(512), 22)
    {
        _font = _manager.loadAndInstantiate("FreeTypeFont");
        if (!_font)
            std::exit(1);

        /* Open the font and fill glyph cache */
        Utility::Resource rs("fonts");
        if (!_font->openSingleData(rs.getRaw(fname), 200.0f)) {
            Error() << "Cannot open font file";
            std::exit(1);
        }

        _font->fillGlyphCache(
            _cache,
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-+,"
            ".\n");

        // This probably should go in some initialization routine somewhere ...
        GL::Renderer::enable(GL::Renderer::Feature::Blending);
        GL::Renderer::setBlendFunction(
            GL::Renderer::BlendFunction::SourceAlpha,
            GL::Renderer::BlendFunction::OneMinusSourceAlpha);
        GL::Renderer::setBlendEquation(
            GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    }

    PluginManager::Manager<Text::AbstractFont> _manager;
    std::unique_ptr<Text::AbstractFont>        _font;
    Text::DistanceFieldGlyphCache              _cache;
};
}