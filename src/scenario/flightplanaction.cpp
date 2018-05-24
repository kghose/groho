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

#include "body.hpp"
#include "flightplanaction.hpp"
#include "scenariolib.hpp"
#include "vector.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

typedef std::string                     str_t;
typedef std::vector<std::string>        strv_t;
typedef FlightPlanAction                fpa_t;
typedef std::optional<FlightPlanAction> fpao_t;

///////////////////////////////
// AVAILABLE ACTIONS (VERBS) //
///////////////////////////////

Body SET_ATTITUDE(Body b, fpa_t& fpa)
{
    b.att = fpa.noun.vector;
    return b;
}

Body SET_ACCEL(Body b, fpa_t& fpa)
{
    if (fpa.noun.scalar > b.max_acc) {
        fpa.noun.scalar = b.max_acc;
        LOG_S(WARNING) << fpa.line_no << ": SET_ACCEL for " << b.name
                       << " exceeds max_acc";
    }
    b.acc = fpa.noun.scalar;
    return b;
}

///////////////////////////////

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
          fpa.verb        = SET_ACCEL;
          fpa.noun.scalar = stof(tokens[2]);
          return fpa;
      } }
};

std::optional<FlightPlanAction>
parse_line_into_action(std::string line, size_t line_no)
{

    std::vector<std::string> tokens = split(line);

    FlightPlanAction fpa;
    fpa.jd      = stof(tokens[0]);
    fpa.line_no = line_no;

    if (actions.count(tokens[1])) {
        return actions[tokens[1]](tokens, fpa);
    } else {
        LOG_S(ERROR) << "No such flight plan action: " << tokens[1];
        return {};
    }
}
}