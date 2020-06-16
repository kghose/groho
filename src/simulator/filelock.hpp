/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Simple semaphoring via locked files
*/

#pragma once

#include <string>

namespace groho {

class FileLock {
public:
    FileLock(std::string outdir);
    ~FileLock();

private:
    std::string lock_fname;
};
}
