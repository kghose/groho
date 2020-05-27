/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE.

Some utilities for parsing input files.
*/
#pragma once

// TODO: Move this under scenario/

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "line.hpp"

namespace fs = std::filesystem;

namespace groho {

std::optional<Lines> load_input_file(const fs::path& path);

const std::string wspace = " \t\n\r\f\v";

std::vector<std::string>
split_string(const std::string& s, const std::string& sep = wspace);
}
