/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Renders a text label
*/

#pragma once

#include <Magnum/GL/Renderer.h>
#include <Magnum/Shaders/DistanceFieldVector.h>

#include "camera.hpp"
#include "font.hpp"

namespace sim {

using namespace Magnum;

class TextBase {

public:
    TextBase(Font& font);
    virtual ~TextBase() = default;

    TextBase& set_text(std::string str);
    TextBase& set_size(float s);
    TextBase& set_color(Color3 col);

    virtual void draw(const Camera& camera) = 0;

protected:
    std::unique_ptr<Text::Renderer2D> _text;
    Shaders::DistanceFieldVector2D    _shader;

    Color3 _color;
    float  _size;
};

class Label2D : public TextBase {

public:
    Label2D(Font& font);
    Label2D& set_pos(const Vector2& p);
    void     draw(const Camera& camera);

private:
    Shaders::DistanceFieldVector2D _shader;

    Vector2 _pos;
};

class Billboard : public TextBase {

public:
    Billboard(Font& font);
    Billboard& set_pos(const Vector3& v);
    void       draw(const Camera& camera);

private:
    Shaders::DistanceFieldVector3D _shader;

    Vector3 _pos;
};
}