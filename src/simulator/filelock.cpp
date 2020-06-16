/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Simple semaphoring via locked files
*/

#include "filelock.hpp"

#include <fcntl.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

namespace groho {

enum Action { lock = 1, unlock = 2 };

void operate_lock_file(const std::string& lock_fname, const Action& action)
{
    struct flock lock;
    lock.l_start  = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len    = 0;

    int fd;

    switch (action) {

    case Action::lock:
        fd          = open(lock_fname.c_str(), O_RDWR | O_CREAT, 0666);
        lock.l_type = F_WRLCK; /* Test for any lock on any part of file. */
        break;

    case Action::unlock:
        fd          = open(lock_fname.c_str(), O_RDWR);
        lock.l_type = F_UNLCK;
        break;
    }

    while (fcntl(fd, F_SETLK, &lock) == -1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

FileLock::FileLock(std::string outdir)
{
    lock_fname = outdir + "/sim.lock";
    operate_lock_file(lock_fname, Action::lock);
}

FileLock::~FileLock() { operate_lock_file(lock_fname, Action::unlock); }
}