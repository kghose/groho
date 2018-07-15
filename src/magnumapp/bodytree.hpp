/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Organizes all the bodies (as ids) in a tree that reflects how we can select them
*/

#pragma once

#include <memory>
#include <vector>

#include "buffer.hpp"
#include "groho.hpp"

namespace sim {

struct BodyTree {
    BodyTree()
    {
        // Categories are
        // ships, barycenters, mercury, venus, earth, mars, jupiter, saturn,
        // nepture, uranus, pluto, asteroid, comets
        for (int i = 0; i < 13; i++) {
            tree.push_back(std::vector<spkid_t>());
        }
    }

    void add(spkid_t sid)
    {
        int id = sid.id;

        size_t category_id = 0;
        if (id < 0)
            category_id = 0;
        else if ((0 <= id) && (id <= 10))
            category_id = 1;
        else if ((100 < id) && (id < 1000)) {
            category_id = (id / 100) + 1;
        } else if (2000000 <= id)
            category_id = 11;
        else
            category_id = 12;

        tree[category_id].push_back(id);
    }

    std::optional<spkid_t> change_cat(int sign)
    {
        size_t orig_cat_id = cat_id;
        while (1) {
            cat_id = (13 + cat_id + sign) % 13;
            if (tree[cat_id].size() > 0) {
                item_id = 0;
                return tree[cat_id][0];
            }
            if (cat_id == orig_cat_id)
                return {};
        }
    }

    std::optional<spkid_t> change_item(int sign)
    {
        size_t item_cnt = tree[cat_id].size();
        return (item_cnt + item_id + sign) % item_cnt;
    }

    std::vector<std::vector<spkid_t>> tree;

    size_t cat_id  = 0;
    size_t item_id = 0;
};
}
