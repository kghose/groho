/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Orrery built with SPK files
*/

#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <stdlib.h>
#include <string>
#include <unordered_set>
#include <vector>

#include "spk.hpp"
#include "tokens.hpp"
#include "units.hpp"

namespace groho {

namespace fs = std::filesystem;

struct OrreryObject {
    std::shared_ptr<Ephemeris> ephemeris;
    size_t                     parent_idx;
    bool                       gravitational_body = true;
};

class Orrery {

public:
    enum StatusCode { OK = 0, WARNING, ERROR };
    Orrery() { ; }
    Orrery(J2000_s begin, J2000_s end, const KernelTokens& kernel_tokens);

    StatusCode            status() { return _status; }
    void                  set_to(J2000_s t, v3d_vec_t& pos);
    std::vector<NAIFbody> list_objects();
    std::vector<size_t>   index_of_gravitational_objects();

private:
    std::vector<OrreryObject> objects;
    StatusCode                _status;
};

std::vector<OrreryObject> load_orrery_objects(
    J2000_s             begin,
    J2000_s             end,
    const KernelTokens& kernel_tokens,
    Orrery::StatusCode& status);

}