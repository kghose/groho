/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This is the information overlay that we place over the window
*/

#include "overlay.hpp"

namespace sim {

Overlay::Overlay()
{
    label     = std::unique_ptr<Label2D>(new Label2D(font));
    billboard = std::unique_ptr<Billboard>(new Billboard(font));
}

void Overlay::draw(const Camera& camera)
{
    label->set_text(std::to_string(t_s));
    label->draw(camera);
}
}