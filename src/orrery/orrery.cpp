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
ephemera in the correct order (centers come before targets).

For each kernel file, load all or some bodies and note their reference centers.
Put the data into a tree as we go, with centers being the parents of targets.
Do a breadth wise traversal and place all the bodies in order.
*/

Orrery::Orrery(J2000_s begin, J2000_s end, const Kernels& kernels)
{
    objects = load_orrery_objects(begin, end, kernels, _status);
}

struct _Body {
    std::shared_ptr<Ephemeris>           ephemeris;
    std::unordered_map<NAIFbody, _Body*> children;
    NAIFbody                             parent;
};

void print_objects_to_debug(const std::vector<OrreryObject>& objects);

std::vector<OrreryObject> load_orrery_objects(
    J2000_s             begin,
    J2000_s             end,
    const Kernels&      kernels,
    Orrery::StatusCode& status)
{
    status = Orrery::StatusCode::OK;
    std::unordered_map<NAIFbody, _Body> bodies;
    bodies[NAIFbody(0)] = _Body();

    for (const auto& kernel : kernels) {
        auto _spk = SpkFile::load(kernel.path);
        if (!_spk) {
            LOG_S(ERROR) << "Unable to load " << kernel.path;
            status = Orrery::StatusCode::WARNING;
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
                status = Orrery::StatusCode::ERROR;
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
            status = Orrery::StatusCode::ERROR;
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

    print_objects_to_debug(objects);

    return objects;
}

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

void Orrery::set_to(J2000_s t, v3d_vec_t& pos) {}
}