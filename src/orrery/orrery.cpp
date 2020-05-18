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

void print_objects_to_debug(const std::vector<OrreryObject>& objects)
{
    for (size_t i = 1; i < objects.size(); i++) {
        auto& obj    = objects[i];
        auto  center = obj.parent_idx == 0
            ? "SSB"
            : std::to_string(objects[obj.parent_idx].ephemeris->target_code);

        LOG_S(INFO) << "[" << i << "]" << obj.ephemeris->target_code << " -> "
                    << "[" << obj.parent_idx << "]" << center;
    }
}

std::optional<Orrery>
Orrery::load(J2000_s begin, J2000_s end, const Kernels& kernels)
{

    Orrery orrery;
    orrery.objects = load_orrery_objects(begin, end, kernels);
    print_objects_to_debug(orrery.objects);

    return orrery;
}

struct _Body {
    std::shared_ptr<Ephemeris>           ephemeris;
    std::unordered_map<NAIFbody, _Body*> children;
    NAIFbody                             parent;
};

std::vector<OrreryObject>
load_orrery_objects(J2000_s begin, J2000_s end, const Kernels& kernels)
{
    std::unordered_map<NAIFbody, _Body> bodies;
    bodies[NAIFbody(0)] = _Body();

    for (const auto& kernel : kernels) {
        auto _spk = SpkFile::load(kernel.path);
        if (!_spk) {
            LOG_S(ERROR) << "Unable to load " << kernel.path;
            continue;
        }

        auto& spk = *_spk;
        for (auto [code, summary] : spk.summaries) {
            if (bodies.find(code) != bodies.end()) {
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

            // todo: return ephemeris as shared pointer
            bodies[code] = _Body{ std::make_shared<Ephemeris>(
                                      *(spk.load_ephemeris(code, begin, end))),
                                  {},
                                  NAIFbody(summary.center_id) };
        }
    }

    for (auto& [code, body] : bodies) {
        if (int(code) == 0) {
            continue;
        }
        if (bodies.find(body.parent) == bodies.end()) {
            LOG_S(ERROR) << "Could not find center for " << int(code);
            continue;
        }
        bodies[body.parent].children[code] = &bodies[code];
    }

    std::vector<OrreryObject> objects;
    objects.push_back({ nullptr, 0 });

    std::unordered_map<NAIFbody, size_t> map_code_to_index;
    map_code_to_index[NAIFbody(0)] = 0;

    std::deque<_Body*> q = { &bodies[NAIFbody(0)] };
    while (!q.empty()) {
        auto this_node = q.front();
        q.pop_front();
        for (auto [code, child] : this_node->children) {
            map_code_to_index[code] = objects.size();
            objects.push_back(
                { child->ephemeris, map_code_to_index[child->parent] });
            q.push_back(child);
        }
    }

    return objects;
}

void Orrery::set_to(J2000_s t, v3d_vec_t& pos) {}
}