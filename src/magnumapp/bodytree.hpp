/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Organizes all the bodies (as ids) in a tree that reflects how we can select them
*/

#pragma once

#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>

#include "naifbody.hpp"

namespace sim {

// For the planets and satellites we have a specific way we organize the body
// tree. For the asteroids, coments and ships , we add them in the order they
// are added to the buffer.
struct BodyTree {

    static const int NEXTCAT  = +1;
    static const int PREVCAT  = -1;
    static const int NEXTBODY = +1;
    static const int PREVBODY = -1;

    BodyTree()
    {
        tree.push_back({ { 0, "SSB" } });
        cat_id  = 0;
        body_id = 0;
    }

    BodyTree(const std::unordered_set<NAIFbody>& bodies_present)
    {
        std::vector<NAIFbody> ships;
        for (auto b : bodies_present) {
            if (b.is_ship()) {
                ships.push_back(b);
            }
        }
        tree.push_back(ships);

        tree.push_back({ { 0, "SSB" } });
        cat_id  = 1;
        body_id = 0;

        // mercury, venus, earth, mars, jupiter, saturn, nepture,
        // uranus, pluto
        for (size_t _id = 1; _id < 10; _id++) {
            std::vector<NAIFbody> this_planet; // yes, Pluto too ...

            int  planet_id = _id * 100 + 99;
            auto idx       = bodies_present.find(planet_id);
            if (idx != bodies_present.end()) {
                this_planet.push_back(*idx);
            }
            for (int sat_id = _id * 100; sat_id < planet_id; sat_id++) {
                auto idx = bodies_present.find(sat_id);
                if (idx != bodies_present.end()) {
                    this_planet.push_back(*idx);
                }
            }
            tree.push_back(this_planet);
        }

        std::vector<NAIFbody> asteroids;
        for (auto b : bodies_present) {
            if (b.is_asteroid()) {
                asteroids.push_back(b);
            }
        }
        tree.push_back(asteroids);

        std::vector<NAIFbody> comets;
        for (auto b : bodies_present) {
            if (b.is_comet()) {
                comets.push_back(b);
            }
        }
        tree.push_back(comets);
    }

    NAIFbody change_cat(int sign)
    {
        while (true) {
            cat_id = (tree.size() + cat_id + sign) % tree.size();
            if (tree[cat_id].size() > 0) {
                body_id = 0;
                return get_body();
            }
        }
    }

    NAIFbody change_item(int sign)
    {
        size_t item_cnt = tree[cat_id].size();

        body_id = (item_cnt + *body_id + sign) % item_cnt;
        return get_body();
    }

    NAIFbody get_body() { return tree[cat_id][*body_id]; }

    std::vector<std::vector<NAIFbody>> tree;

    size_t                cat_id = 0;
    std::optional<size_t> body_id;
};
}
