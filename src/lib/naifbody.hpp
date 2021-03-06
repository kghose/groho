/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Type for NAIF IDs
*/

#pragma once

#include <string>

namespace groho {

struct NAIFbody {
    int code;

    NAIFbody(int a = 0)
        : code(a)
    {
    }

    constexpr explicit operator int() const { return code; }

    bool is_barycenter() const { return (0 <= code) && (code < 10); }
    bool is_ship() const { return code < 0; }
    bool is_planet() const
    {
        return (code > 10) && (code < 1000) && ((code % 100) == 99);
    }
    bool is_satellite() const
    {
        return (code > 10) && (code < 1000) && ((code % 100) != 99);
    }
    bool is_asteroid() const { return (code > 2000000); }
    bool is_comet() const { return (code > 1000000) && (code <= 2000000); }

    NAIFbody barycenter() const { return code / 100; }

    bool operator<(const NAIFbody& s) const { return (code < (int)s); }
    bool operator==(const NAIFbody& s) const { return (code == (int)s); }
};
}

namespace std {
template <> struct hash<groho::NAIFbody> {
    size_t operator()(const groho::NAIFbody& body) const
    {
        return hash<int>()((int)body);
    }
};
}
