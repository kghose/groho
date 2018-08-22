/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Any data we need to protect with a mutex we wrap with this.
*/

#pragma once

#include <mutex>
#include <utility>

namespace sim {

template <typename T> class Locked {
public:
    Locked(T& _object, std::mutex& _mutex)
        : object(_object)
        , lock(_mutex)
    {
    }

    T&                          object;
    std::lock_guard<std::mutex> lock;
};

template <typename T> class Lockable {
public:
    Locked<T>       borrow() { return Locked(object, mutex); }
    Locked<const T> borrow() const { return Locked(object, mutex); }

private:
    T                  object;
    mutable std::mutex mutex;
};
}