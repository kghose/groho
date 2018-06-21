/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

This file descibes the verb/noun combinations available to control spacecraft
via the flight plans.
*/

#include <cctype>
#include <cmath>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "body.hpp"
#include "flightplanaction.hpp"
#include "scenariolib.hpp"
#include "state.hpp"
#include "vector.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

double do_this_first(std::numeric_limits<double>::lowest());

/*
bool set_key_value(std::optional<KeyValue> kv)
{
    std::unordered_map<sst, std::function<void(sst)>> keyword_map{

        { "name", [=](sst v) { body.name                       = v; } },
        { "max-acceleration", [=](sst v) { body.max_acc        = stof(v); } },
        { "max-fuel", [=](sst v) { body.max_fuel               = stof(v); } },
        { "fuel-cons-rate", [=](sst v) { body.fuel_cons        = stof(v); } },
        { "orbiting", [=](sst v) { initial_orbit               = v; } },
        { "flight-state", [=](sst v) { body.state.flight_state = FALLING; } },
        // TODO: handle landed state
        { "position", [=](sst v) { body.state.pos              = stoV(v); } },
        { "velocity", [=](sst v) { body.state.vel              = stoV(v); } },
        { "attitude", [=](sst v) { body.state.att              = stoV(v); } },
        //
        { "fuel", [=](sst v) { body.state.fuel                 = stof(v); } },
        { "acc", [=](sst v) { body.state.acc                   = stof(v); } },

    };

    if (keyword_map.count(kv->key)) {
        keyword_map[kv->key](kv->value);
        return true;
    } else {
        return false;
    }
};*/

/*FlightPlanAction::FlightPlanAction(
    size_t ship_idx, std::string fname, size_t line_no, double jd)
    : ship_idx(ship_idx)
    , fname(fname)
    , line_no(line_no)
    , t_s(jd2s(jd))
{
    done = false;
}*/

// TODO: refactor flight plan actions to remove duplication of code/concerns
// between action constructor and map that constructs action from line

///////////////////////////////
// AVAILABLE ACTIONS (VERBS) //
///////////////////////////////

/*
struct SET_NAME : public FlightPlanAction {
    std::string name;

    fpa_uptr_t construct(
        size_t                   ship_idx,
        std::string              fname,
        size_t                   line_no,
        double                   jd,
        std::vector<std::string> tokens){

        FlightPlanAction action = { ship_idx, fname, line_no, jd2s(jd), tokens }

        fpa_uptr_t action(new SET_NAME(ship_idx, fname, line_no, ))
    }

    SET_ATTITUDE(double jd, size_t line_no, Vector v)
        : FlightPlanAction(jd, line_no)
        , att(v.normed())
    {
    }

    void operator()(State& state)
    {
        state.ships[ship_idx]
            .property
            .

            body.state.att
            = att;
        active = false;
        DLOG_S(INFO) << line_no << ": " << body.name << " att -> "
                     << body.state.att;
    }
};*/

/*
struct SET_ATTITUDE : public FlightPlanAction {
    const Vector att;

    SET_ATTITUDE(double jd, size_t line_no, Vector v)
        : FlightPlanAction(jd, line_no)
        , att(v.normed())
    {
    }

    void operator()(Body& body, const WorldState& ws)
    {
        body.state.att = att;
        active         = false;
        DLOG_S(INFO) << line_no << ": " << body.name << " att -> "
                     << body.state.att;
    }
};

struct SET_ACCEL : public FlightPlanAction {
    const double acc;

    SET_ACCEL(double jd, size_t line_no, double a)
        : FlightPlanAction(jd, line_no)
        , acc(a)
    {
    }

    void operator()(Body& body, const WorldState& ws)
    {
        double _acc = acc;
        if (_acc > body.max_acc) {
            _acc = body.max_acc;
            LOG_S(WARNING) << line_no << ": SET_ACCEL for " << body.name
                           << " exceeds max_acc";
        }

        body.state.acc = _acc;
        active         = false;
        DLOG_S(INFO) << line_no << ": " << body.name << " acc -> "
                     << body.state.acc;
    }
};

// Utility to compute deltaV given GM, R and V
Vector parking_deltaV(double GM, Vector R, Vector Vs)
{
    double r_mod       = R.norm();
    Vector minus_R_hat = R.normed() * -1;
    Vector Vs_hat      = Vs.normed();
    Vector Uo_hat      = cross(minus_R_hat, Vs_hat);
    Vector Vo_hat      = cross(Uo_hat, minus_R_hat);
    Vector Vo          = Vo_hat * std::sqrt(GM / r_mod);
    return Vo - Vs;
}

// V0.0
// Once we come within R km of a planet, maneuver to set V to match orbital VO
// For each step
// 1. Compute the velocity vector required to convert V to VO (deltaV)
// 2. Set attitude along deltaV
// 3. Set engine accel (A) such that deltaV = 0.5 * A * Tstep^2 or maxA
// 4. Once |V| is within epsilon (e) of |VO| deactivate this program
struct PARK_IN_ORBIT : public FlightPlanAction {
    const double epsilon = 0.001; // 1m/s tolerance
    const double R_capture;
    const int    spkid;
    size_t       obv_idx
        = -1; // The index into the ws.obv vector for body with given spkid
    bool parking_maneuver_started = false;

    PARK_IN_ORBIT(double jd, size_t line_no, int spkid, double R)
        : FlightPlanAction(jd, line_no)
        , R_capture(R)
        , spkid(spkid)
    {
    }

    void operator()(Body& body, const WorldState& ws)
    {
        if (obv_idx == -1) {
            obv_idx = orrery::spkid_to_orrery_index(ws.obv, spkid);
            if (obv_idx == -1) {
                LOG_S(ERROR) << line_no << ": PARK_IN_ORBIT : Can't find "
                             << spkid << " in Orrery";
                active = false;
                return;
            }
        }

        auto const& ob = ws.obv[obv_idx];

        Vector R = body.state.pos - ob.state.pos;
        // DLOG_S(INFO) << R.norm();
        if (R.norm() > R_capture) {
            return;
        }

        if (!parking_maneuver_started) {
            LOG_S(INFO) << line_no << ": Starting to park " << body.name
                        << " -> " << ob.name;
            parking_maneuver_started = true;
        }

        Vector deltaV = parking_deltaV(
            ob.GM, body.state.pos - ob.state.pos, body.state.vel);

        body.state.att = deltaV.normed();
        // deltaV = 0.5 * A * Tstep^2
        double acc = deltaV.norm() / (0.5 * ws.t_s * ws.t_s);

        if (acc > body.max_acc) {
            body.state.acc = body.max_acc;
            LOG_S(WARNING) << line_no << ": acceleration required for parking "
                           << body.name << " exceeds max_acc";
        } else {
            body.state.acc = acc;
        }

        if (deltaV.norm() < epsilon) {
            active = false;
            LOG_S(INFO) << line_no << ": Ship " << body.name
                        << " successfully parked around " << ob.name;
        }
    }
};*/

///////////////////////////////

typedef std::string                      str_t;
typedef std::vector<std::string>         strv_t;
typedef std::unordered_map<str_t, str_t> params_t;
typedef FlightPlanAction                 fpa_t;

template <class T> using ptr_t = std::shared_ptr<T>;
// typedef std::unique_ptr uptr_t;
// use of class template 'unique_ptr' requires template arguments; argument
// deduction not allowed in typedef

struct SET_ATTITUDE : public FlightPlanAction {

    SET_ATTITUDE(const FPAD& _fpad)
        : FlightPlanAction(_fpad)
    {
    }

    static ptr_t<SET_ATTITUDE> construct(const FPAD& _fpad, params_t params)
    {
        if (params.size() != 3) {
            LOG_S(ERROR) << _fpad.fname << ": Line: " << _fpad.line_no
                         << "Need three elements for vector";
            return {};
        }

        auto action = ptr_t<SET_ATTITUDE>(new SET_ATTITUDE(_fpad));
        action->att
            = Vector{ stof(params["x"]), stof(params["y"]), stof(params["z"]) };

        return action;
    }

    void operator()(State& state)
    {
        state.ships[p.ship_idx].state.att = att;

        p.done = true;
    }

    Vector att;
};

std::unordered_map<str_t, std::function<fpap_t(const FPAD&, params_t&)>>
    available_actions{ { "set-attitude", SET_ATTITUDE::construct } };

/*
std::unordered_map<str_t, std::function<fpaptr_t(double, size_t, strv_t)>>
    actions{

        { "set-attitude",
          [](double jd, size_t line_no, strv_t tokens) -> fpaptr_t {
              if (tokens.size() != 5) {
                  LOG_S(ERROR) << "Can't parse attitude vector";
                  return {};
              }
              return fpaptr_t(new SET_ATTITUDE(
                  jd,
                  line_no,
                  { stod(tokens[2]), stod(tokens[3]), stod(tokens[4]) }));
          } },

        { "set-accel",
          [](double jd, size_t line_no, strv_t tokens) -> fpaptr_t {
              if (tokens.size() != 3) {
                  LOG_S(ERROR) << "Can't parse accelaration fraction";
                  return {};
              }
              return fpaptr_t(new SET_ACCEL(jd, line_no, stod(tokens[2])));
          } },

        { "park",
          [](double jd, size_t line_no, strv_t tokens) -> fpaptr_t {
              if (tokens.size() != 4) {
                  LOG_S(ERROR) << "Can't parse park";
                  return {};
              }
              return fpaptr_t(new PARK_IN_ORBIT(
                  jd, line_no, stoi(tokens[2]), stod(tokens[3])));
          } },

    };*/

// For the preamble
//   <action name> <argument1>
// For the plan proper
//   <timestamp> <action name> <argument1> <argument2> ...
fpap_t parse_line_into_action(
    size_t                   ship_idx,
    std::string              fname,
    size_t                   line_no,
    std::vector<std::string> tokens)
{
    double jd = do_this_first;
    if (tokens[0] != "-") {
        jd = stod(tokens[0]);
    }

    if (available_actions.count(tokens[1])) {
        params_t params;

        for (size_t i = 2; i < tokens.size(); i++) {
            auto kv = get_named_parameter(tokens[i]);
            if (kv) {
                params[kv->key] = kv->value;
            }
        }

        return available_actions[tokens[1]](
            FPAD{ ship_idx, fname, line_no, jd, false }, params);

    } else {
        LOG_S(ERROR) << fname << ": " << line_no
                     << ": Unknown action: " << tokens[1];
        return {};
    }
}
}