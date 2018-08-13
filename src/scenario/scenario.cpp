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

Scenario::Scenario(
    const Configuration& new_config, std::shared_ptr<Scenario> old_scenario)
{
    if (old_scenario == nullptr) {
        _simulation.simulation_serial = 1;
    } else {
        _simulation.simulation_serial = old_scenario->simulation_serial() + 1;
    }

    // Right now, we only do Orrery caching. In more sophisticated
    // implementations we would compare the configurations and retain components
    // of the old simulation too ...

    bool reload_orrery = (_orrery == nullptr)
        || (_config.orrery_fnames != new_config.orrery_fnames)
        || ((new_config.begin_s < _orrery->begin_s)
            || (_orrery->end_s < new_config.end_s));

    if (reload_orrery) {
        _orrery = std::shared_ptr<SpkOrrery>(new SpkOrrery(
            new_config.orrery_fnames,
            new_config.begin_s - new_config.step_s,
            new_config.end_s));
    } else {
        _orrery = old_scenario->orrery();
    }

    for (auto& o : _orrery->get_bodies()) {
        _simulation.system.push_back({ o, {} });
    }

    int default_ship_code = -1000;
    // https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html#Spacecraft
    for (auto& fp_name : new_config.flightplan_fnames) {
        auto ship = parse_ship_properties(fp_name, --default_ship_code);
        if (ship) {
            _simulation.fleet.push_back({ ship->property, {} });
            actions.splice(
                actions.end(),
                parse_ship_actions(
                    fp_name, _simulation.fleet.size() - 1, ship->property));
        }
    }
    actions.sort(fpa_order);

    LOG_S(INFO) << _simulation.system.size() << " rocks in simulation.";
    LOG_S(INFO) << _simulation.fleet.size() << " spaceships in simulation.";
    LOG_S(INFO) << "Loaded " << actions.size() << " actions";

    _config = new_config;
    _valid  = true;
}

// Gives the properties and state of the entire simulation at a given time
// point. This is expensive because of the interpolation and copying
void Scenario::get_snapshot_at(double t_s, Objects<SnapShot>&) const { ; }

// The passed in function has the opportunity to copy over the simulation
void Scenario::read(
    const std::vector<SimulationSegment>&  ss,
    std::function<void(const Simulation&)> f) const
{
    std::lock_guard<std::mutex> lock(_simulation.buffer_mutex);
    f(_simulation);
}

// Possibly very expensive operation. A display element can ask for a
// simulation object that only consists of parts of the simulation and that
// has been interpolated
void Scenario::interpolate(
    const std::vector<SimulationSegment>&  ss,
    std::function<void(const Simulation&)> f)
{
    std::lock_guard<std::mutex> lock(_simulation.buffer_mutex);
    f(_simulation);
}

} // namespace sim