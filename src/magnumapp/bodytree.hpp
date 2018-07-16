/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Organizes all the bodies (as ids) in a tree that reflects how we can select them
*/

#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "buffer.hpp"
#include "groho.hpp"

namespace sim {

// For the planets and satellites we have a specific way we organize the body
// tree. For the asteroids, coments and ships , we add them in the order they
// are added to the buffer.
struct BodyTree {

    enum BodyCategory { SHIP, BARYCENTER, PLANET, ASTEROID, COMET };
    static const int NEXTCAT  = +1;
    static const int PREVCAT  = -1;
    static const int NEXTBODY = +1;
    static const int PREVBODY = -1;

    BodyTree() = default;

    BodyTree(const std::unordered_set<int> bodies_present)
    {
        std::vector<spkid_t> ships;
        for (auto b : bodies_present) {
            if (get_category(b) == SHIP) {
                ships.push_back(b);
            }
        }
        tree.push_back(ships);

        // mercury, venus, earth, mars, jupiter, saturn, nepture,
        // uranus, pluto
        for (size_t _cat_id = 1; _cat_id < 10; _cat_id++) {
            std::vector<spkid_t> this_planet; // yes, Pluto too ...

            int planet_id = _cat_id * 100 + 99;
            if (bodies_present.count(planet_id) > 0) {
                this_planet.push_back(planet_id);
            }
            for (int sat_id = _cat_id * 100; sat_id < planet_id; sat_id++) {
                if (bodies_present.count(sat_id) > 0) {
                    this_planet.push_back(sat_id);
                }
            }
            tree.push_back(this_planet);
        }

        std::vector<spkid_t> asteroids;
        for (auto b : bodies_present) {
            if (get_category(b) == ASTEROID) {
                asteroids.push_back(b);
            }
        }
        tree.push_back(asteroids);

        std::vector<spkid_t> comets;
        for (auto b : bodies_present) {
            if (get_category(b) == COMET) {
                comets.push_back(b);
            }
        }
        tree.push_back(comets);
    }

    static BodyCategory get_category(spkid_t sid)
    {
        int id = sid.id;

        if (id < 0)
            return SHIP;
        else if ((0 <= id) && (id <= 10))
            return BARYCENTER;
        else if ((100 < id) && (id < 1000)) {
            return PLANET;
        } else if (2000000 <= id)
            return ASTEROID;
        else
            return COMET;
    }

    std::optional<spkid_t> change_cat(int sign)
    {
        size_t orig_cat_id = cat_id;
        while (1) {
            cat_id = (12 + cat_id + sign) % 12;
            if (tree[cat_id].size() > 0) {
                body_id = 0;
                return get_body_id();
            }
            if (cat_id == orig_cat_id)
                return {};
        }
    }

    std::optional<spkid_t> change_item(int sign)
    {
        size_t item_cnt = tree[cat_id].size();
        if (item_cnt == 0)
            return {};
        body_id = (item_cnt + *body_id + sign) % item_cnt;

        return get_body_id();
    }

    std::optional<spkid_t> get_body_id()
    {
        if (body_id) {
            return tree[cat_id][*body_id];
        } else {
            return {};
        }
    }

    std::vector<std::vector<spkid_t>> tree;

    size_t                cat_id = 0;
    std::optional<size_t> body_id;
};
}
