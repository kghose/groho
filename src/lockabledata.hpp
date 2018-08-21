/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Any data we need to protect with a mutex we wrap with this. It's semantics are
that of a pointer - we use * to recover the original object
*/

#pragma once

#include <mutex>

namespace sim {

template <typename T> class LockedReference {
public:
    LockedReference(T& _object, std::mutex& _mutex)
        : object(_object)
        , lock(_mutex)
    {
    }

    T& object;

private:
    std::lock_guard<std::mutex> lock;
};

template <typename T> class LockableData {
public:
    LockedReference<T>       get() { return LockedReference<T>(data, mutex); }
    LockedReference<const T> get() const
    {
        return LockedReference<const T>(data, mutex);
    }

private:
    T                  data;
    mutable std::mutex mutex;
};
}