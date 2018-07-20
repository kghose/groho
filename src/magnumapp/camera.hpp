/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This is a simple struct to store camera state. The state is passed to renderers
so that they know what camera model to use.
*/

#pragma once

#include <optional>

#include <Magnum/Magnum.h>
#include <Magnum/Math/Math.h>
#include <Magnum/Math/Matrix4.h>

#include "bodytree.hpp"
#include "groho.hpp"

namespace sim {

using namespace Magnum;
using namespace Math::Literals;

class Camera {
public:
    Deg   fov   = 35.0_degf;
    float scale = 1.0;

    int viewport_x, viewport_y;

    BodyTree body_tree;

    std::optional<spkid_t> center_id = spkid_t(0);
    double                 t_s;

    Vector3 center = { 0, 0, 0 };
    Deg     az     = 0.0_degf;
    Deg     el     = 0.0_degf;

    void set_viewport(int x, int y)
    {
        viewport_x   = x;
        viewport_y   = y;
        aspect_ratio = (float)x / (float)y;
    }

    Matrix4 get_matrix() const
    {
        return Matrix4::perspectiveProjection(fov, aspect_ratio, front, back)
            * Matrix4::translation(Vector3::zAxis(-10.0f))
            * Matrix4::rotationX(el) * Matrix4::rotationZ(az)
            * Matrix4::rotationX(-23.5_degf) * Matrix4::scaling(Vector3(scale))
            * Matrix4::translation(-center);
    }

    Matrix4 get_billboard_matrix() const
    {
        return Matrix4::perspectiveProjection(fov, aspect_ratio, front, back)
            * Matrix4::translation(Vector3::zAxis(-10.0f))
            * Matrix4::scaling(Vector3(scale)) * Matrix4::translation(-center);
    }

private:
    float front        = 0.01f;
    float back         = 100.0f;
    float aspect_ratio = 2.0;
};
}