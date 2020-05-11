/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Orrery built with SPK files
*/

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

struct Node {
    SpkFile*                            spk_file = nullptr;
    std::unordered_map<NAIFbody, Node*> children;
};

std::optional<Orrery> Orrery::load(
    std::vector<NAIFbody>    codes,
    std::vector<std::string> file_names,
    J2000_s                  begin,
    J2000_s                  end)
{

    std::unordered_map<NAIFbody, Node> tree;
    tree[NAIFbody(0)] = Node();
    for (const auto& code : codes) {
        tree[code] = Node();
    }

    std::vector<SpkFile> spk_files;
    for (const auto& kernel_file : file_names) {
        auto _spk = SpkFile::load(kernel_file);
        if (_spk) {
            spk_files.push_back(*_spk);
        }
    }

    bool make_pass = true;
    while (make_pass) {
        make_pass = false;
        for (auto& spk : spk_files) {
            for (auto [_, summary] : spk.summaries) {
                NAIFbody _target_code = NAIFbody(summary.target_id);
                auto     _ti          = tree.find(_target_code);
                if (_ti != tree.end()) {
                    if (_ti->second.spk_file == nullptr) {
                        if ((summary.begin_second <= begin)
                            && (summary.end_second >= end)) {
                            _ti->second.spk_file = &spk;

                        } else {
                            LOG_S(ERROR) << spk.file_name + ": "
                                    + get_body_name(_target_code)
                                    + ": time range out of bounds";
                            return {};
                        }
                    }
                }
                NAIFbody _center_code = NAIFbody(summary.center_id);
                auto     _ci          = tree.find(_center_code);
                if (_ci == tree.end()) {
                    tree[_center_code] = Node();
                    make_pass          = true;
                }
                tree[_center_code].children[_target_code] = &tree[_target_code];
            }
        }
    }

    Orrery orrery;
    orrery.ephemera.reserve(tree.size());
}

void Orrery::set_to(J2000_s t, v3d_vec_t& pos) {}
}