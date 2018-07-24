/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenient collecting point for some program wide definitions and constants
*/
#pragma once

namespace sim {

// I wanted a way to distinguish functions that took an index into an array
// and functions that actually take a spk_id.
struct spkid_t {
    int         id;
    std::string name;
    spkid_t(int a = 0, std::string _name = "SSB")
    {
        id   = a;
        name = _name;
    }
    bool operator<(const spkid_t& s) const { return (id < s.id); }
    bool operator==(const spkid_t& s) const { return (id == s.id); }
};

namespace globals {
const double scale = 10 * 149'597'870.700; // 10 AU
}
}

namespace std {
template <> struct hash<sim::spkid_t> {
    size_t operator()(const sim::spkid_t& obj) const
    {
        return hash<int>()(obj.id);
    }
};
}
