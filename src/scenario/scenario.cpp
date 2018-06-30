/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This defines some functions for the simulation scenario data structure
*/
#include <iostream>
#include <set>

#include "scenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

bool time_range_extended(
    const orrery::SpkOrrery& orrery, const Configuration& config)
{
    return (config.begin_s < orrery.begin_s) || (orrery.end_s < config.end_s);
}

bool orrery_changed(const Configuration& a, const Configuration& b)
{
    return (a.orrery_fnames != b.orrery_fnames);
}

std::optional<Body> parse_ship_properties(std::string fname, int ship_idx)
{
    BodyConstant       p;
    ShipCharacteristic ch;
    BodyState          s;

    p.code = ship_idx;

    typedef std::string sst;

    std::unordered_map<sst, std::function<void(sst)>> keyword_map{

        { "name", [&](sst v) { p.name                  = v; } },
        { "max-acceleration", [&](sst v) { ch.max_acc  = stof(v); } },
        { "max-fuel", [&](sst v) { ch.max_fuel         = stof(v); } },
        { "burn-rate", [&](sst v) { ch.burn_rate       = stof(v); } },
        { "flight-state",
          [&]([[maybe_unused]] sst v) { s.flight_state = FALLING; } },

        // TODO: handle landed state
        { "position", [&](sst v) { s.pos = stoV(v); } },
        { "velocity", [&](sst v) { s.vel = stoV(v); } },
        { "attitude", [&](sst v) { s.att = stoV(v); } },
        //
        { "fuel", [&](sst v) { s.fuel    = stof(v); } },
        { "acc", [&](sst v) { s.acc      = stof(v); } },

    };

    auto flt_plan_file = ScenarioFile::open(fname);
    if (!flt_plan_file) {
        LOG_S(ERROR) << fname << ": flight plan not found";
        return {};
    }

    LOG_S(INFO) << "Loading ship: " << fname;

    for (auto line = flt_plan_file->next(); line;
         line      = flt_plan_file->next()) {

        auto tokens = split(*line);
        if (could_be_a_number(tokens[0])) {
            continue;
        }

        auto kv = get_key_value(*line);
        if (keyword_map.count(kv->key)) {
            keyword_map[kv->key](kv->value);
        } else {
            LOG_S(ERROR) << fname << ": line " << flt_plan_file->line_no
                         << ": unknown key: " << kv->key;
        }
    }

    return Body{ p, ch, s };
}

fpapl_t parse_ship_actions(std::string fname, size_t ship_idx)
{
    auto flt_plan_file = ScenarioFile::open(fname);
    if (!flt_plan_file) {
        LOG_S(ERROR) << fname << ": flight plan not found";
        return {};
    }

    LOG_S(INFO) << "Loading actions: " << fname;
    fpapl_t actions;

    for (auto line = flt_plan_file->next(); line;
         line      = flt_plan_file->next()) {

        auto tokens = split(*line);
        if (!could_be_a_number(tokens[0])) {
            continue;
        }

        actions.push_back(parse_line_into_action(
            ship_idx, fname, flt_plan_file->line_no, tokens));
    }

    actions.remove_if([](fpap_t& p) { return p == nullptr; });

    LOG_S(INFO) << "Loaded " << actions.size() << " actions";

    return actions;
}

void Scenario::from(const Configuration& new_config)
{
    bool reload_orrery = (config.orrery_fnames != new_config.orrery_fnames)
        || ((new_config.begin_s < orrery.begin_s)
            || (orrery.end_s < new_config.end_s));

    if (reload_orrery) {
        orrery = orrery::SpkOrrery(
            new_config.orrery_fnames, new_config.begin_s, new_config.end_s);
    }

    ships.clear();
    actions.clear();
    int default_ship_code = -1000;
    // https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html#Spacecraft
    for (auto& fp_name : new_config.flightplan_fnames) {
        auto ship = parse_ship_properties(fp_name, --default_ship_code);
        if (ship) {
            ships.push_back(*ship);
            actions.splice(
                actions.end(), parse_ship_actions(fp_name, ships.size() - 1));
        }
    }
    actions.sort(fpa_order);
    LOG_S(INFO) << "Loaded " << actions.size() << " actions";

    config = new_config;
    valid  = true;
}

} // namespace sim