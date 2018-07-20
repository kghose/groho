/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Renders a text label
*/

#include "textlabel.hpp"

namespace sim {

TextBase::TextBase(Font& font)
{
    _text.reset(new Text::Renderer2D(
        *(font._font), font._cache, 1.0f, Text::Alignment::LineLeft));
    _text->reserve(
        40, GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);

    _shader.bindVectorTexture(font._cache.texture());

    _color = { 1, 1, 1 };
    _size  = 0.1;
}

TextBase& TextBase::set_text(std::string str)
{
    _text->reserve(
        str.size(), GL::BufferUsage::DynamicDraw, GL::BufferUsage::StaticDraw);
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

Label2D::Label2D(Font& font)
    : TextBase(font)
{
}

Label2D& Label2D::set_pos(const Vector2& p)
{
    _pos = p;
    return *this;
}

void Label2D::draw(const Camera& camera)
{
    _shader
        .setTransformationProjectionMatrix(
            Matrix3::scaling(Vector2(_size)) * Matrix3::translation(_pos))
        .setColor(_color);
    _text->mesh().draw(_shader);
}

Billboard::Billboard(Font& font)
    : TextBase(font)
{
}

Billboard& Billboard::set_pos(const Vector3& p)
{
    _pos = p;
    return *this;
}

void Billboard::draw(const Camera& camera)
{
    _shader.setTransformationProjectionMatrix(camera.get_billboard_matrix())
        .setColor(_color);
    _text->mesh().draw(_shader);
}

// TextLabel& TextLabel::set_pos2d(Vector2d p)
// {
//     _pos_2d = p;
//     _mode   = TEXT_2D;
//     return *this;
// }

// TextLabel& TextLabel::set_pos3d_as_fixed_size_billboard(Vector3d p)
// {
//     _pos_3d = p;
//     _mode   = TEXT_3D_FIXED_BILLBOARD;
//     return *this;
// }

// TextLabel& TextLabel::set_pos3d_as_scaled_billboard(Vector3d p)
// {
//     _pos_3d = p;
//     _mode   = TEXT_3D_SCALED_BILLBOARD;
//     return *this;
// }

// // TODO:
// // 1. Inversely scale with viewport size to maintain font size constancy
// // 2. Figure out screen coordinates (probably from viewport)
// // 3. Learn how to project from 3D to 2D for billboards of two types
// void TextLabel::draw(const Camera& camera)
// {
//     // _transformation = Matrix3::scaling(Vector2(_size))
//     //     * Matrix3::translation(Vector2::yAxis(-2.0f))
//     //     * Matrix3::translation(Vector2::xAxis(-3.0f));
//     // _shader.setTransformationProjectionMatrix(_projection *
//     _transformation)
//     //     .setColor(_color);
//     // _text->mesh().draw(_shader);
//     _shader2.setTransformationProjectionMatrix(camera.get_billboard_matrix())
//         .setColor(_color);
//     _text->mesh().draw(_shader2);
// }
}