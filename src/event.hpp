/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Events are markers added - always by annotations, sometimes by flight plans -
that carry a space-time coordinate and a display text. They are meant to
indicate interesting points in the simulation.
*/

#pragma once

#include "naifbody.hpp"
#include "vector.hpp"

namespace sim {

struct Thing {
    NAIFbody naif;
    Vector   pos;
    Vector   vel;
};

struct Event {
    double               t_s;
    std::optional<Thing> thing[2];
    enum Kind { SIGNAL, BURN_START, BURN_END, DIST, VEL } kind;
    std::string message;
};
}