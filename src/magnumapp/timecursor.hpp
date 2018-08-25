/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Renders a text label
*/

#pragma once

#include <algorithm>

namespace sim {

struct TimeCursor {
    double current_s = 0;
    double delta_s   = 86400; // how far we advance when we scroll in time
    double sim_begin_s;
    double sim_progress_s;
    double sim_end_s;

    void set_range(double _t0, double _t1)
    {
        sim_begin_s = _t0;
        sim_end_s   = _t1;
        current_s   = sim_begin_s;
    }

    void forward()
    {
        current_s = std::min(sim_progress_s, current_s + delta_s);
    }
    void backward() { current_s = std::max(sim_begin_s, current_s - delta_s); }

    void set(double t_s)
    {
        current_s = std::min(sim_progress_s, std::max(sim_begin_s, t_s));
    }
};
}
