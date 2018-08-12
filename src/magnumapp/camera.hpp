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
#include "groholib.hpp"
#include "naifbody.hpp"

namespace sim {

using namespace Magnum;
using namespace Math::Literals;

struct TimeCursor {
    double current_s = 0;
    double delta_s   = 86400; // how far we advance when we scroll in time
    double sim_begin_s;
    double sim_end_s;

    void set_range(double _t0, double _t1)
    {
        sim_begin_s = _t0;
        sim_end_s   = _t1;
        current_s   = std::min(sim_end_s, std::max(sim_begin_s, current_s));
    }

    void forward() { current_s = std::min(sim_end_s, current_s + delta_s); }
    void backward() { current_s = std::max(sim_begin_s, current_s - delta_s); }
};

struct SpaceCursor {
    float scale = 1.0;
    Deg   az    = 0.0_degf;
    Deg   el    = 0.0_degf;

    NAIFbody center_id = { 0, "SSB" };
    Vector3  center    = { 0, 0, 0 };
};

struct View {
    Deg   fov   = 35.0_degf;
    float front = 0.01f;
    float back  = 100.0f;

    int   width, height;
    float aspect_ratio = 2.0;

    void set_viewport(int x, int y)
    {
        width        = x;
        height       = y;
        aspect_ratio = (float)x / (float)y;
    }
};

class Camera {
public:
    void set_time_range(double _t0, double _t1)
    {
        time_cursor.set_range(_t0, _t1);
    }

    void step_forward_in_time() { time_cursor.forward(); }
    void step_backward_in_time() { time_cursor.backward(); }

    void increase_az_by(Deg delta_az)
    {
        space_cursor.az
            = Deg(std::fmod((float)(space_cursor.az + delta_az), 360));
    }

    void increase_el_by(Deg delta_el)
    {
        space_cursor.el
            = Deg(std::fmod((float)(space_cursor.el + delta_el), 360));
    }

    void set_viewport(int x, int y) { view.set_viewport(x, y); }

    void set_body_tree(BodyTree bt) { body_tree = bt; }

    void next_category()
    {
        space_cursor.center_id = body_tree.change_cat(BodyTree::NEXTCAT);
    }
    void prev_category()
    {
        space_cursor.center_id = body_tree.change_cat(BodyTree::PREVCAT);
    }
    void next_body()
    {
        space_cursor.center_id = body_tree.change_item(BodyTree::NEXTBODY);
    }
    void prev_body()
    {
        space_cursor.center_id = body_tree.change_item(BodyTree::PREVBODY);
    }

    void zoom_in() { space_cursor.scale *= 1.1; }
    void zoom_out() { space_cursor.scale /= 1.1; }

    void set_center_id(const NAIFbody cid) { space_cursor.center_id = cid; }
    void set_center(const Vector3& p) { space_cursor.center = p; }

    NAIFbody center_id() const { return space_cursor.center_id; }
    double   current_s() const { return time_cursor.current_s; }

    const View& get_view() const { return view; }

    std::string to_string() const
    {
        return space_cursor.center_id.name + ": "
            + std::to_string(s2jd(time_cursor.current_s)) + " JD: ("
            + std::to_string((float)space_cursor.az) + ", "
            + std::to_string((float)space_cursor.el) + ")";
    }

    Matrix4 get_matrix() const
    {
        return Matrix4::perspectiveProjection(
                   view.fov, view.aspect_ratio, view.front, view.back)
            * Matrix4::translation(Vector3::zAxis(-10.0f))
            * Matrix4::rotationX(space_cursor.el)
            * Matrix4::rotationZ(space_cursor.az)
            * Matrix4::rotationX(ecliptic_correction)
            * Matrix4::scaling(Vector3(space_cursor.scale))
            * Matrix4::translation(-space_cursor.center);
    }

    Matrix4 get_billboard_matrix(Vector3 pos) const
    {
        Vector4 point = get_billboard_translation(pos);

        return Matrix4::perspectiveProjection(
                   view.fov, view.aspect_ratio, view.front, view.back)
            * Matrix4::translation(Vector3::zAxis(-10.0f))
            * Matrix4::translation(Vector3(point.x(), point.y(), point.z()))
            * Matrix4::scaling(Vector3(space_cursor.scale));
    }

    Matrix4 get_unscaled_billboard_matrix(Vector3 pos) const
    {
        Vector4 point = get_billboard_translation(pos);

        return Matrix4::perspectiveProjection(
                   view.fov, view.aspect_ratio, view.front, view.back)
            * Matrix4::translation(Vector3::zAxis(-10.0f))
            * Matrix4::translation(Vector3(point.x(), point.y(), point.z()));
    }

    Vector4 get_billboard_translation(Vector3 pos) const
    {
        return Matrix4::rotationX(space_cursor.el)
            * Matrix4::rotationZ(space_cursor.az)
            * Matrix4::rotationX(ecliptic_correction)
            * Matrix4::scaling(Vector3(space_cursor.scale))
            * Matrix4::translation(-space_cursor.center)
            * Vector4{ pos.x(), pos.y(), pos.z(), 1 };
    }

private:
    TimeCursor  time_cursor;
    SpaceCursor space_cursor;
    View        view;
    BodyTree    body_tree;

    Deg ecliptic_correction = -23.5_degf;
};
}