/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Orrery built with SPK files
*/

#include <deque>
#include <unordered_map>

#include "bodyconstant.hpp"
#include "orrery.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

/*
Given a list of bodies to display and a list of SPK files, load up and store the
ephemera in the correct order (centers come before targets) and load any
ephemera that are needed for centers but are not explicitly mentioned.

As long as we have to make passes:
  For each SPK file:
    For each body:
      If the body is in the tree:
        If the body needs to be loaded:
          Mark it for loading
      else if the body is in the list:
        Add it to the tree and mark it for loading
        If the center is not in the tree:
          Add it to the tree
          Mark the tree for another pass

For each node in the tree:
  If there is a node not marked for loading:
    Flag error and exit

Create an array the length of the tree.

Starting from the root (which is 0, SSB):
  Do a DF traversal:
    Read in ephemera into the array in order.
*/

std::optional<Orrery>
Orrery::load(J2000_s begin, J2000_s end, const Kernels& kernels)
{
    Orrery orrery;

    orrery.bodies[NAIFbody(0)] = Body{};

    for (const auto& kernel : kernels) {
        auto _spk = SpkFile::load(kernel.path);
        if (!_spk) {
            LOG_S(ERROR) << "Unable to load " << kernel.path;
            continue;
        }

        auto& spk = *_spk;
        for (auto [code, summary] : spk.summaries) {
            if (orrery.bodies.find(code) != orrery.bodies.end()) {
                continue;
            }
            if ((kernel.codes.size() > 0)
                && (kernel.codes.find(code) == kernel.codes.end())) {
                continue;
            }
            if (!summary.valid_time_range(begin, end)) {
                LOG_S(ERROR) << spk.path;
                continue;
            }

            orrery.bodies[code] = Body{ *(spk.load_ephemeris(code, begin, end)),
                                        {},
                                        NAIFbody(summary.center_id) };
        }
    }

    for (auto& [code, body] : orrery.bodies) {
        if (orrery.bodies.find(body.parent) == orrery.bodies.end()) {
            LOG_S(ERROR) << "Could not find center for " << int(code);
            continue;
        }
        orrery.bodies[body.parent].children[code] = &body;
    }

    return orrery;
}

void Orrery::set_to(J2000_s t, v3d_vec_t& pos) {}
}