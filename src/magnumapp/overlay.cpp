/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This is the information overlay that we place over the window
*/

#include "overlay.hpp"

namespace sim {

Overlay::Overlay()
{
    label = std::unique_ptr<Label2D>(
        new Label2D(font, Text::Alignment::LineLeft));
    (*label).set_anchor(Label2D::BOTTOM_LEFT).set_pos({ 10, 10 }).set_size(50);
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
        status_string = "Running: " + std::to_string(t_s);
        break;

    default:
        status_string = "Unknown";
        break;
    }

    label->set_text(status_string);
    label->draw(camera);
}
}