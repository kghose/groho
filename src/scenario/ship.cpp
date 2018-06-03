/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file contains definitions for the scenario flightplan data structure.
The code takes care of parsing flight plans from file, representing
them as lists of actions.
*/

#include <fstream>

#include "ship.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

// This expects the Orrery to be computed with velocities
// We place the ship in a pro-grade orbit at the given altitude
// A (km) = desired altitude of ship
// r (km) = radius of body
// Rsun   = vector from SSB to Body
// Vbody  = velocity vector of Body (rel to SSB)
// U_hat  = Vbody X -Rsun ("normal to orbital plane")
// V_hat  = -Rsun X U = tangent to planetary surface ("pro-grade vector")
//
// Ship needs to be positioned at Rsun + (A + r) along Rsun
// Ship needs a velocity along V_hat of magnitude sqrt(GM/(A + r)) relative to
// Body. Which means DON'T FORGET TO ADD Vbody to V :D
void Ship::set_initial_state_as_orbiting(const WorldState& ws)
{
    std::vector<std::string> tokens = split(initial_orbit);
    if (tokens.size() < 2) {
        LOG_S(ERROR) << "Initial orbit specification needs two components";
        return;
    }

    size_t SSB_idx = orrery::spkid_to_orrery_index(ws.obv, 10);
    if (SSB_idx == -1) {
        LOG_S(ERROR) << "Can't find SSB in Orrery when trying to set initial "
                        "state as orbiting";
        return;
    }

    size_t B_idx = orrery::spkid_to_orrery_index(ws.obv, stoi(tokens[0]));
    if (B_idx == -1) {
        LOG_S(ERROR)
            << "Can't find " << tokens[0]
            << " in Orrery when trying to set initial state as orbiting";
        return;
    }

    double A       = stod(tokens[1]);
    double r       = ws.obv[B_idx].r;
    Vector Rsun    = ws.obv[B_idx].state.pos - ws.obv[SSB_idx].state.pos;
    Vector Vbody   = ws.obv[B_idx].state.vel;
    Vector U_hat   = cross(Vbody, Rsun * -1).normed();
    Vector V_hat   = cross(Rsun * -1, U_hat).normed();
    body.state.pos = ws.obv[B_idx].state.pos + (Rsun.normed() * (A + r));
    body.state.vel = (V_hat * std::sqrt(ws.obv[B_idx].GM / (A + r))) + Vbody;
    body.state.att = V_hat; // Nice to set this too

    DLOG_S(INFO) << V_hat;

    body.state.flight_state = FALLING;
}

std::optional<Ship> parse_flight_plan(std::string fname, int default_code)
{

    std::ifstream cfile(fname);
    int           line_no = 0;
    std::string   line;
    Ship          ship;
    bool          header_section = true;

    if (!cfile) {
        LOG_S(ERROR) << fname << ": Flight plan not found";
        return {};
    }

    // Some miscellaneous defaults
    ship.body.body_type = SPACESHIP;
    ship.body.color     = 0x0000aa;
    ship.body.code      = default_code;

    while (std::getline(cfile, line)) {
        line_no++;

        line = trim_whitespace(trim_comments(line));

        if (line.length() == 0) // Ignore empty lines
            continue;

        if (header_section) {
            std::optional<KeyValue> kv = get_key_value(line);
            if (kv) {
                if (kv->key == "plan") {
                    header_section = false;
                } else {
                    if (!ship.set_key_value(kv)) {
                        LOG_S(WARNING) << "Line " << line_no << ": Unknown key "
                                       << kv->key << ": ignoring";
                    }
                }
            } else {
                LOG_S(ERROR)
                    << "Error parsing line: " << line_no << " (" << line << ")";
            }
        } else {
            auto fpa = parse_line_into_action(line, line_no);
            if (fpa) {
                ship.plan.push_back(fpa);
            }
        }
    }
    return ship;
}
}