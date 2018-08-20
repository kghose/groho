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

std::optional<SnapShot<ShipLike>>
parse_ship_properties(std::string fname, int ship_idx)
{
    SnapShot<ShipLike> ship;

    ship.property.naif = { ship_idx, std::to_string(ship_idx) };

    typedef std::string sst;

    std::unordered_map<sst, std::function<void(sst)>> keyword_map{

        { "name", [&](sst v) { ship.property.naif.name = v; } },
        { "color",
          [&](sst v) { ship.property.color = stoul(v, nullptr, 16); } },
        { "max-acceleration", [&](sst v) { ship.property.max_acc = stof(v); } },
        { "max-fuel", [&](sst v) { ship.property.max_fuel        = stof(v); } },
        { "burn-rate", [&](sst v) { ship.property.burn_rate      = stof(v); } },

        // It's much too random to try and place a spacecraft absolutely in the
        // solar system but we let you do it
        { "position", [&](sst v) { ship.state.pos = stoV(v); } },
        { "velocity", [&](sst v) { ship.state.vel = stoV(v); } },
        { "attitude", [&](sst v) { ship.state.att = stoV(v); } },
        { "acc", [&](sst v) { ship.state.acc      = stof(v); } },
        { "fuel", [&](sst v) { ship.state.fuel    = stof(v); } },
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

    return ship;
}

fpapl_t parse_ship_actions(
    std::string fname, size_t ship_idx, ShipLike::Property property)
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
            ship_idx, fname, flt_plan_file->line_no, tokens, property));
    }

    actions.remove_if([](fpap_t& p) { return p == nullptr; });

    LOG_S(INFO) << "Loaded " << actions.size() << " actions";

    return actions;
}

std::shared_ptr<const SpkOrrery> load_orrery(
    const Configuration&             new_config,
    const Configuration&             old_config,
    std::shared_ptr<const SpkOrrery> old_orrery)
{
    std::shared_ptr<const SpkOrrery> new_orrery;

    bool reload_orrery = (old_orrery == nullptr)
        || (old_config.orrery_fnames != new_config.orrery_fnames)
        || ((new_config.begin_s < old_orrery->begin_s)
            || (old_orrery->end_s < new_config.end_s));

    if (reload_orrery) {
        new_orrery = std::shared_ptr<SpkOrrery>(new SpkOrrery(
            new_config.orrery_fnames,
            new_config.begin_s - new_config.step_s,
            new_config.end_s));
    } else {
        new_orrery = old_orrery;
    }
    return new_orrery;
}

std::optional<ShipWithActions>
load_ship(std::string fp_name, int ship_code, int ship_idx)
{
    ShipWithActions swa;

    auto ship = parse_ship_properties(fp_name, ship_code);
    if (ship) {
        swa.ship    = *ship;
        swa.actions = parse_ship_actions(fp_name, ship_idx, ship->property);
        return swa;
    } else {
        return {};
    }
}

/*
const std::vector<RockLike::Property> Scenario::system_list() const
{
    std::vector<RockLike::Property> _p;
    for (auto const& r : _simulation.system.bodies) {
        _p.push_back(r.property);
    }
    return _p;
}

const std::vector<ShipLike::Property> Scenario::fleet_list() const
{
    std::vector<ShipLike::Property> _p;
    for (auto const& s : _simulation.fleet.bodies) {
        _p.push_back(s.property);
    }
    return _p;
}
*/
} // namespace sim