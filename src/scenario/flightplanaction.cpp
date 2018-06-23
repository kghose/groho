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
#include "orrery.hpp"
#include "scenariolib.hpp"
#include "state.hpp"
#include "vector.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

double do_this_first(std::numeric_limits<double>::lowest());

///////////////////////////////
// AVAILABLE ACTIONS (VERBS) //
///////////////////////////////

/*


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

// Initializer actions, should only be allowed at start of simulation ////////

struct INITIAL_ORBIT : public FlightPlanAction {

    INITIAL_ORBIT(const FPAD& _fpad)
        : FlightPlanAction(_fpad)
    {
    }

    static ptr_t<INITIAL_ORBIT> construct(const FPAD& _fpad, params_t params)
    {
        if (_fpad.t_s > do_this_first) {
            LOG_S(ERROR) << _fpad.fname << ": Line: " << _fpad.line_no
                         << ": initial orbit action can only be defined to "
                            "occur at start of simulation. Time must be '-'";
        }

        try {
            auto action    = ptr_t<INITIAL_ORBIT>(new INITIAL_ORBIT(_fpad));
            action->spk_id = stoi(params["id"]);
            action->alt    = stod(params["alt"]);
            return action;
        } catch (std::exception) {
            LOG_S(ERROR)
                << _fpad.fname << ": Line: " << _fpad.line_no
                << ": Need two elements for initial orbit: id:399 alt:3000";
            return {};
        }
    }

    void operator()(State& state)
    {
        size_t SSB_idx = orrery::spkid_to_orrery_index(state.orrery, 10);
        if (SSB_idx == -1UL) {
            LOG_S(ERROR)
                << "Can't find SSB in Orrery when trying to set initial "
                   "state as orbiting";
            done = true;
            return;
        }

        size_t B_idx = orrery::spkid_to_orrery_index(state.orrery, spk_id);
        if (B_idx == -1UL) {
            LOG_S(ERROR)
                << "Can't find " << spk_id
                << " in Orrery when trying to set initial state as orbiting";
            return;
        }

        double A = alt;
        double r = state.orrery[B_idx].property.r;
        Vector Rsun
            = state.orrery[B_idx].state.pos - state.orrery[SSB_idx].state.pos;
        Vector Vbody = state.orrery[B_idx].state.vel;
        Vector U_hat = cross(Vbody, Rsun * -1).normed();
        Vector V_hat = cross(Rsun * -1, U_hat).normed();
        state.ships[p.ship_idx].state.pos
            = state.orrery[B_idx].state.pos + (Rsun.normed() * (A + r));
        state.ships[p.ship_idx].state.vel
            = (V_hat * std::sqrt(state.orrery[B_idx].property.GM / (A + r)))
            + Vbody;
        state.ships[p.ship_idx].state.att = V_hat; // Nice to set this too

        state.ships[p.ship_idx].state.flight_state = FALLING;

        done = true;
    }

    int    spk_id;
    double alt;
};

// Basic Actions ///////////////////////////////////////////////////////////

struct SET_ATTITUDE : public FlightPlanAction {

    SET_ATTITUDE(const FPAD& _fpad)
        : FlightPlanAction(_fpad)
    {
    }

    static ptr_t<SET_ATTITUDE> construct(const FPAD& _fpad, params_t params)
    {

        try {
            auto action = ptr_t<SET_ATTITUDE>(new SET_ATTITUDE(_fpad));
            action->att = Vector{ stof(params["x"]),
                                  stof(params["y"]),
                                  stof(params["z"]) };

            return action;
        } catch (std::exception) {
            LOG_S(ERROR) << _fpad.fname << ": Line: " << _fpad.line_no
                         << ": Need three floats for attitude vector: eg: "
                            "x:1.1 y:0.4 z:0.2";
            return {};
        }
    }

    void operator()(State& state)
    {
        state.ships[p.ship_idx].state.att = att;
        DLOG_S(INFO) << state.ships[p.ship_idx].property.name
                     << " attitude set";
        done = true;
    }

    Vector att;
};

struct SET_ACCEL : public FlightPlanAction {

    SET_ACCEL(const FPAD& _fpad)
        : FlightPlanAction(_fpad)
    {
    }

    static ptr_t<SET_ACCEL> construct(const FPAD& _fpad, params_t params)
    {
        try {
            auto action = ptr_t<SET_ACCEL>(new SET_ACCEL(_fpad));
            action->acc = stof(params["acc"]);
            return action;
        } catch (std::exception) {
            LOG_S(ERROR) << _fpad.fname << ": Line: " << _fpad.line_no
                         << ": Need one element for accel eg: acc:23.45";
            return {};
        }
    }

    void operator()(State& state)
    {
        double _acc = acc;
        if (_acc > state.ships[p.ship_idx].property.max_acc) {
            _acc = state.ships[p.ship_idx].property.max_acc;
            LOG_S(WARNING) << p.line_no << ": SET_ACCEL for "
                           << state.ships[p.ship_idx].property.name
                           << " exceeds max_acc";
        }

        state.ships[p.ship_idx].state.acc = _acc;
        DLOG_S(INFO) << state.ships[p.ship_idx].property.name
                     << " acceleration set";
        done = true;
    }

    double acc;
};

std::unordered_map<str_t, std::function<fpap_t(const FPAD&, params_t&)>>
    available_actions{ { "set-attitude", SET_ATTITUDE::construct },
                       { "set-accel", SET_ACCEL::construct },
                       { "initial-orbit", INITIAL_ORBIT::construct } };

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
            FPAD{ ship_idx, fname, line_no, jd }, params);

    } else {
        LOG_S(ERROR) << fname << ": " << line_no
                     << ": Unknown action: " << tokens[1];
        return {};
    }
}
}