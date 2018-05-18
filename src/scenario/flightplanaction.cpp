/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file descibes the verb/noun combinations available to control spacecraft
via the flight plans.
*/

#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>

#include "flightplanaction.hpp"
#include "scenariolib.hpp"
#include "vector.hpp"

namespace sim {

// Each verb gets a meaningfully named enum here
enum Verb : unsigned int { SET_ATTITUDE, SET_ACCEL };

typedef std::string                     str_t;
typedef std::vector<std::string>        strv_t;
typedef FlightPlanAction                fpa_t;
typedef std::optional<FlightPlanAction> fpao_t;

std::unordered_map<str_t, std::function<fpao_t(strv_t, fpa_t)>> actions{

    { "set-attitude",
      [](strv_t tokens, FlightPlanAction fpa) -> fpao_t {
          if (tokens.size() != 5) {
              LOG_S(ERROR) << "Can't parse attitude vector";
              return {};
          }
          fpa.verb = SET_ATTITUDE;
          fpa.noun.vector
              = { stof(tokens[2]), stof(tokens[3]), stof(tokens[4]) };
          return fpa;
      } },

    { "set-accel",
      [](strv_t tokens, FlightPlanAction fpa) -> fpao_t {
          if (tokens.size() != 3) {
              LOG_S(ERROR) << "Can't parse accelaration fraction";
              return {};
          }
          fpa.verb          = SET_ACCEL;
          fpa.noun.fraction = stof(tokens[2]);
          return fpa;
      } }
};

std::optional<FlightPlanAction> parse_line_into_action(std::string line)
{

    std::vector<std::string> tokens = split(line);

    FlightPlanAction fpa;
    fpa.jd = stof(tokens[0]);

    if (actions.count(tokens[1])) {
        return actions[tokens[1]](tokens, fpa);
    } else {
        LOG_S(ERROR) << "No such flight plan action: " << tokens[1];
        return {};
    }
}
}