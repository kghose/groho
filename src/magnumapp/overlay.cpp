/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This is the information overlay that we place over the window
*/

#include "overlay.hpp"

namespace sim {

Overlay::Overlay()
{
    status_label = std::unique_ptr<Label2D>(
        new Label2D(font, Text::Alignment::LineRight));
    (*status_label)
        .set_anchor(Label2D::BOTTOM_RIGHT)
        .set_pos({ 10, 10 })
        .set_size(30);

    view_label
        = std::unique_ptr<Label2D>(new Label2D(font, Text::Alignment::TopLeft));
    (*view_label)
        .set_anchor(Label2D::TOP_LEFT)
        .set_pos({ 10, 10 })
        .set_size(30);

    billboard = std::unique_ptr<Billboard>(
        new Billboard(font, Text::Alignment::LineLeft));
}

void Overlay::draw(const Camera& camera)
{
    std::string status_string;

    switch (status) {
    case Simulator::WAITING:
        status_string = "Waiting";
        break;

    case Simulator::LOADING:
        status_string = "Loading";
        break;

    case Simulator::RUNNING:
        status_string = "Running: " + std::to_string(jd);
        break;

    default:
        status_string = "Unknown";
        break;
    }

    status_label->set_text(status_string);
    status_label->draw(camera);

    view_label->set_text(
        "View: " + camera_center + ": " + std::to_string(s2jd(camera.current_s))
        + " JD: (" + std::to_string((float)camera.az) + ", "
        + std::to_string((float)camera.el) + ")");
    view_label->draw(camera);
}
}