/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

Code to parse scenario files and flight plans and load in the Orrery and Ship
definitions.
*/
#include <iostream>
#include <set>

#include "scenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

bool could_be_a_property_line(std::string line)
{
    return line.find("=") != std::string::npos;
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

std::optional<LoadedShip> load_ship(std::string fp_name, int ship_code)
{
    // Would have been nice to use structured bindngs like this, but a
    // problem in the C++ specification prevents this
    // https://stackoverflow.com/a/46115028/2512851
    // LoadedShip ship;
    // auto& [property, flight_plan] = ship;

    ShipLike::Property property;
    FlightPlan         flight_plan;

    typedef std::string sst;

    property.naif = { ship_code, std::to_string(ship_code) };

    std::unordered_map<sst, std::function<void(sst)>> process_key_value{

        { "name", [&](sst v) { property.naif.name = v; } },
        { "color", [&](sst v) { property.color    = stoul(v, nullptr, 16); } },
        { "max-acceleration", [&](sst v) { property.max_acc = stof(v); } },
        { "max-fuel", [&](sst v) { property.max_fuel        = stof(v); } },
        { "burn-rate", [&](sst v) { property.burn_rate      = stof(v); } },

    };

    auto flt_plan_file = ScenarioFile::open(fp_name);
    if (!flt_plan_file) {
        LOG_S(ERROR) << fp_name << ": flight plan not found";
        return {};
    }

    LOG_S(INFO) << "Loading ship and flight plan: " << fp_name;

    for (auto line = flt_plan_file->next(); line;
         line      = flt_plan_file->next()) {

        if (could_be_a_property_line(*line)) {

            auto kv = get_key_value(*line);
            if (process_key_value.count(kv->key)) {
                process_key_value[kv->key](kv->value);
            } else {
                LOG_S(ERROR) << fp_name << ":" << flt_plan_file->line_no
                             << " Property? " << *line;
            }

        } else {

            auto fpa = parse_line_into_action(*line);

            if (fpa) {

                fpa->flightplan_fname = fp_name;
                fpa->command_string   = *line;
                fpa->line_no          = flt_plan_file->line_no;
                flight_plan.push_back(std::move(fpa));

            } else {
                LOG_S(ERROR) << fp_name << ":" << flt_plan_file->line_no
                             << " Action? " << *line;
            }
        }
    }

    return LoadedShip{ property, std::move(flight_plan) };
}

} // namespace sim