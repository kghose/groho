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

struct Node;
typedef std::unordered_map<NAIFbody, Node>  Tree;
typedef std::unordered_map<NAIFbody, Node*> Children;

struct Node {
    const SpkFile* spk_file = nullptr;
    Children       children;
};

typedef std::unordered_map<NAIFbody, Node> Tree;

Tree initialize_tree(const std::vector<NAIFbody>& codes)
{
    Tree tree;
    tree[NAIFbody(0)] = Node();
    for (const auto& code : codes) {
        tree[code] = Node();
    }
    return tree;
}

spk_vec_t load_spk(const std::vector<std::string>& file_names)
{
    spk_vec_t spk_files;
    for (const auto& kernel_file : file_names) {
        auto _spk = SpkFile::load(kernel_file);
        if (_spk) {
            spk_files.push_back(*_spk);
        }
    }
    return spk_files;
}

Node* node_to_attach_to(const Summary& summary, Tree& tree)
{
    NAIFbody _target_code = NAIFbody(summary.target_id);
    auto     _ti          = tree.find(_target_code);
    if (_ti != tree.end()) {
        if (_ti->second.spk_file == nullptr) {
            return &(_ti->second);
        }
    }
    return nullptr;
}

bool inserted_parent_in_tree(const Summary& summary, Tree& tree)
{
    bool     inserted     = false;
    NAIFbody _target_code = NAIFbody(summary.target_id);
    NAIFbody _center_code = NAIFbody(summary.center_id);

    auto _ci = tree.find(_center_code);
    if (_ci == tree.end()) {
        tree[_center_code] = Node();
        inserted           = true;
    }
    tree[_center_code].children[_target_code] = &tree[_target_code];
    return inserted;
}

bool arrange_tree(
    const spk_vec_t& spk_files, Tree& tree, J2000_s begin, J2000_s end)
{
    bool make_another_pass = true, ok = true;
    while (make_another_pass) {
        for (auto& spk : spk_files) {
            for (auto [_, summary] : spk.summaries) {
                auto target_node = node_to_attach_to(summary, tree);
                if (target_node != nullptr) {
                    if (summary.valid_time_range(begin, end)) {
                        (*target_node).spk_file = &spk;
                        if (inserted_parent_in_tree(summary, tree)) {
                            make_another_pass = false;
                        }
                    } else {
                        LOG_S(ERROR) << spk.file_name;
                        ok = false;
                    }
                }
            }
        }
    }
    return ok;
}

std::optional<Orrery> Orrery::load(
    std::vector<NAIFbody>    codes,
    std::vector<std::string> file_names,
    J2000_s                  begin,
    J2000_s                  end)
{
    Tree tree = initialize_tree(codes);

    spk_vec_t spk_files = load_spk(file_names);
    if (!arrange_tree(spk_files, tree, begin, end)) {
        return {};
    }

    Orrery orrery;
    orrery.ephemera.resize(tree.size() - 1);
    Node* root = &tree[NAIFbody(0)];

    std::deque<Node*> q       = { root };
    size_t            eph_idx = 0;
    LOG_S(INFO) << q.size();

    while (!q.empty()) {
        auto this_node = q.front();
        q.pop_front();
        for (auto [code, child] : this_node->children) {
            LOG_S(INFO) << "Loading " << child->spk_file->file_name << ": "
                        << std::to_string(int(code));
            orrery.ephemera[eph_idx]
                = *(child->spk_file->load_ephemeris(code, begin, end));
            q.push_back(child);
        }
    }

    return orrery;
}

void Orrery::set_to(J2000_s t, v3d_vec_t& pos) {}
}