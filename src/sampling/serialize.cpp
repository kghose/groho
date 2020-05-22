/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE
*/

#include <exception>
#include <string>

#include "serialize.hpp"

namespace groho {

Serialize::Serialize(fs::path path, const std::vector<NAIFbody>& objects)
{
    if (!fs::is_directory(path)) {
        throw std::runtime_error("Output path must be directory");
    }

    history.reserve(objects.size());
    for (size_t i = 0; i < objects.size(); i++) {
        auto file = path / (std::to_string(int(objects[i])) + "-pos.bin");
        history.emplace_back(objects[i], file);
    }
}

}
