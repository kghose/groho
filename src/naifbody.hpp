/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Type for NAIF IDs
*/

#pragma once

#include <string>

namespace sim {

struct NAIFcode {
    int value;
    explicit NAIFcode(int a) { value = a; }
    constexpr explicit operator int() const { return value; }
};

struct NAIFbody {
    NAIFcode    code;
    std::string name;
    NAIFbody(NAIFcode a = NAIFcode(0), std::string _name = "SSB")
        : code(a)
    {
        name = _name;
    }
    bool is_barycenter() { return (0 <= int(code)) && (int(code) < 10); }
    bool is_ship() { return int(code) < 0; }
    bool is_planet()
    {
        return (int(code) > 10) && (int(code) < 1000)
            && ((int(code) % 100) == 99);
    }
    bool is_satellite()
    {
        return (int(code) > 10) && (int(code) < 1000)
            && ((int(code) % 100) != 99);
    }
    bool is_asteroid() { return (int(code) > 2000000); }
    bool is_comet() { return (int(code) > 1000000) && (int(code) <= 2000000); }

    bool operator<(const NAIFbody& s) const
    {
        return (int(code) < int(s.code));
    }
    bool operator==(const NAIFbody& s) const
    {
        return (int(code) == int(s.code));
    }
};
}

namespace std {
template <> struct hash<sim::NAIFbody> {
    size_t operator()(const sim::NAIFbody& obj) const
    {
        return hash<int>()(int(obj.code));
    }
};
}
