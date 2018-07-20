/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This is the information overlay that we place over the window
*/

#pragma once

#include "simulator.hpp"
#include "textlabel.hpp"

namespace sim {

class Overlay {
public:
    Overlay();
    void draw(const Camera& camera);

public:
    Simulator::Status status;

    double t_s;

private:
    Font font;

    std::unique_ptr<Label2D>   label;
    std::unique_ptr<Billboard> billboard;
};
}
