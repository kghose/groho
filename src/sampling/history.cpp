/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#include "history.hpp"

namespace groho {

History::~History()
{
    buffer->write(pos()); // This will sometimes give us a duplicate datapoint
                          // right at the end and we are ok with that.
}

void History::sample(const V3d& pos)
{
    _idx++;
    if (_idx > 2) {
        _idx = 0;
    }
    _pos[_idx] = pos;

    if (sampler(pos)) {
        buffer->write(pos);
    }
}

/*

  _idx    0    1    2
    0    P0   P-2  P-1
    1    P-1  P0   P-2
    2    P-2  P-1  P0

    V0  = (P0 - P-1) / dt
    V-1 = (P-1 - P-2) / dt

    A0  = (V0 - V-1) / dt
*/

V3d History::pos() const { return _pos[_idx]; }
V3d History::vel() const
{
    switch (_idx) {
    case 0:
        return (_pos[0] - _pos[2]) / dt;
    case 1:
        return (_pos[1] - _pos[0]) / dt;
    case 2:
        return (_pos[2] - _pos[1]) / dt;
    }
}
V3d History::acc() const
{
    switch (_idx) {
    case 0:
        //                 V0                          V-1
        return ((_pos[0] - _pos[2]) / dt - (_pos[2] - _pos[1]) / dt) / dt;
    case 1:
        //                 V0                          V-1
        return ((_pos[1] - _pos[0]) / dt - (_pos[0] - _pos[2]) / dt) / dt;
    case 2:
        //                 V0                          V-1
        return ((_pos[2] - _pos[1]) / dt - (_pos[1] - _pos[0]) / dt) / dt;
    }
}

}