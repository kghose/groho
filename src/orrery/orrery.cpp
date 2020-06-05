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

Orrery::Orrery(J2000_s begin, J2000_s end, const KernelTokens& kernel_tokens)
{
    objects = load_orrery_objects(begin, end, kernel_tokens, _status);
    set_bodies();
    set_grav_body_idx();
    set_naif_to_idx();
}

void Orrery::pos_at(J2000_s t, v3d_vec_t& pos)
{
    for (size_t i = 1; i < objects.size(); i++) {
        objects[i].ephemeris->eval(t, pos[i - 1]);
    }
    for (size_t i = 1; i < objects.size(); i++) {
        if (objects[i].parent_idx != 0) {
            pos[i - 1] += pos[objects[i].parent_idx - 1];
        }
    }
}

void Orrery::set_bodies()
{
    for (size_t i = 1; i < objects.size(); i++) {
        NAIFbody code = objects[i].ephemeris->target_code;
        try {
            bodies.push_back(body_library.at(code));
        } catch (const std::out_of_range& e) {
            LOG_S(WARNING) << "No GM data for " << int(code);
            bodies.push_back({ code, std::to_string(int(code)) });
        }
    }
}

void Orrery::set_grav_body_idx()
{
    for (size_t i = 1; i < objects.size(); i++) {
        if (objects[i].gravitational_body) {
            grav_body_idx.push_back(i - 1);
        }
    }
}

void Orrery::set_naif_to_idx()
{
    for (size_t i = 1; i < objects.size(); i++) {
        naif_to_idx[objects[i].ephemeris->target_code] = i - 1;
    }
}

struct _Body {
    std::shared_ptr<Ephemeris>           ephemeris;
    std::unordered_map<NAIFbody, _Body*> children;
    NAIFbody                             parent;
};

std::vector<OrreryObject>
traverse_tree_depth_first(std::unordered_map<NAIFbody, _Body>& bodies);

void print_objects_to_debug(const std::vector<OrreryObject>& objects);

std::vector<OrreryObject> load_orrery_objects(
    J2000_s             begin,
    J2000_s             end,
    const KernelTokens& kernel_tokens,
    Orrery::StatusCode& status)
{
    status = Orrery::StatusCode::OK;
    std::unordered_map<NAIFbody, _Body> bodies;
    bodies[NAIFbody(0)] = _Body();

    for (const auto& kernel : kernel_tokens) {
        auto _spk = SpkFile::load(kernel.path);
        if (!_spk) {
            LOG_S(ERROR) << "Unable to load " << kernel.path;
            if (status != Orrery::StatusCode::ERROR) {
                status = Orrery::StatusCode::WARNING;
            }
            continue;
        }

        auto objects_to_find = kernel.codes;

        auto& spk = *_spk;
        for (auto [code, summary] : spk.summaries) {
            if (bodies.find(code) != bodies.end()) {
                continue;
            }
            if ((kernel.codes.size() > 0)
                && (kernel.codes.find(code) == kernel.codes.end())) {
                objects_to_find.erase(code);
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
            objects_to_find.erase(code);
        }

        if (objects_to_find.size() > 0) {
            LOG_S(ERROR) << "Could not find following objects";
            LOG_S(ERROR) << spk.path;
            for (auto& code : objects_to_find) {
                LOG_S(ERROR) << int(code);
            }
            if (status != Orrery::StatusCode::ERROR) {
                status = Orrery::StatusCode::WARNING;
            }
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

    auto objects = traverse_tree_depth_first(bodies);

    print_objects_to_debug(objects);

    return objects;
}

std::vector<OrreryObject>
traverse_tree_depth_first(std::unordered_map<NAIFbody, _Body>& bodies)
{
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
            if (code.is_planet()) {
                auto bary = map_code_to_index.find(code.barycenter());
                if (bary != map_code_to_index.end()) {
                    objects[bary->second].gravitational_body = false;
                }
            }
            q.push_back(child);
        }
    }
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

}