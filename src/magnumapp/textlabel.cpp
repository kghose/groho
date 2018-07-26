/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Renders a text label
*/

#include "textlabel.hpp"

namespace sim {

TextBase::TextBase(Font& font, Text::Alignment ta)
{
    _text.reset(new Text::Renderer2D(*(font._font), font._cache, 1.0f, ta));
    // TODO: Figure out how to dynamically allocate
    _text->reserve(
        100, GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);
    _shader.bindVectorTexture(font._cache.texture());

    _color = { 1, 1, 1 };
    _size  = 100;
}

TextBase& TextBase::set_text(std::string str)
{
    // TODO: Figure out how to dynamically allocate
    if (str.size() > 100)
        str.resize(100);
    _text->render(str);
    return *this;
}

TextBase& TextBase::set_size(float s)
{
    _size = s;
    return *this;
}

TextBase& TextBase::set_color(Color3 col)
{
    _color = col;
    return *this;
}

Label2D::Label2D(Font& font, Text::Alignment ta)
    : TextBase(font, ta)
{
}

Label2D& Label2D::set_anchor(Anchor a)
{
    anchor = a;
    return *this;
}

Label2D& Label2D::set_pos(const Vector2& p)
{
    _pos = p;
    return *this;
}

void Label2D::draw(const Camera& camera)
{
    Vector2d    normalized_pos;
    const View& view = camera.get_view();
    switch (anchor) {
    case BOTTOM_LEFT:
        normalized_pos = { 2 * _pos.x() / view.width - 1.0,
                           2 * _pos.y() / view.height - 1.0 };
        break;
    case BOTTOM_RIGHT:
        normalized_pos = { 1.0 - 2 * _pos.x() / view.width,
                           2 * _pos.y() / view.height - 1.0 };
        break;
    case TOP_LEFT:
        normalized_pos = { 2 * _pos.x() / view.width - 1.0,
                           1.0 - 2 * _pos.y() / view.height };
        break;
    case TOP_RIGHT:
        normalized_pos = { 1.0 - 2 * _pos.x() / view.width,
                           1.0 - 2 * _pos.y() / view.height };
        break;

    default:
        break;
    }

    _shader
        .setTransformationProjectionMatrix(
            Matrix3::translation((Vector2)normalized_pos)
            * Matrix3::scaling({ _size / view.width, _size / view.height }))
        .setColor(_color);
    _text->mesh().draw(_shader);
}

Billboard::Billboard(Font& font, Text::Alignment ta)
    : TextBase(font, ta)
{
}

Billboard& Billboard::set_pos(const Vector3& p)
{
    _pos = p;
    return *this;
}

void Billboard::draw(const Camera& camera)
{
    // _shader.setTransformationProjectionMatrix(camera.get_billboard_matrix())
    //     .setColor(_color);
    // _text->mesh().draw(_shader);
}
}