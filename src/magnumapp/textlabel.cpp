/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Renders a text label
*/

#include "textlabel.hpp"

namespace sim {

TextLabel::TextLabel(Font& font)
{
    _text2.reset(new Text::Renderer2D(
        *(font._font), font._cache, 1.5f, Text::Alignment::LineLeft));
    _text2->reserve(
        40, GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);

    _shader.bindVectorTexture(font._cache.texture());

    _transformation
        = Matrix3::rotation(Deg(0.0f)) * Matrix3::scaling(Vector2(0.5f));
    _projection = Matrix3::scaling(Vector2::yScale(
        Vector2(GL::defaultFramebuffer.viewport().size()).aspectRatio()));

    _color = { 1, 1, 1 };
}

TextLabel& TextLabel::set_text(std::string str)
{
    _text2->reserve(
        str.size(), GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);
    _text2->render(str);
    return *this;
}

TextLabel& TextLabel::set_color(Color3 col)
{
    _color = col;
    return *this;
}

TextLabel& TextLabel::set_pos(Vector3d p) { return *this; }

void TextLabel::draw(const Camera& camera)
{
    _shader.setTransformationProjectionMatrix(_projection * _transformation)
        .setColor(_color);
    _text2->mesh().draw(_shader);
}
}