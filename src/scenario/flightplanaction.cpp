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

typedef std::string                      str_t;
typedef std::vector<std::string>         strv_t;
typedef std::unordered_map<str_t, str_t> params_t;
typedef FlightPlanAction                 fpa_t;

///////////////////////////////
// AVAILABLE ACTIONS (VERBS) //
///////////////////////////////

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
            done = true;
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

// Basic Actions ///////////////////////////////////////////////////////////////

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
        if (state.t_s < p.t_s)
            return;

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
        if (state.t_s < p.t_s)
            return;

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

// Advanced actions ////////////////////////////////////////////////////////////

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
    const double          epsilon = 0.001; // 1m/s tolerance
    double                R_capture;
    std::optional<double> last_t_s;
    int                   spk_id;
    size_t                obv_idx
        = -1; // The index into the ws.obv vector for body with given spkid
    bool parking_maneuver_started = false;

    PARK_IN_ORBIT(const FPAD& _fpad)
        : FlightPlanAction(_fpad)
    {
    }

    static ptr_t<PARK_IN_ORBIT> construct(const FPAD& _fpad, params_t params)
    {
        try {
            auto action = ptr_t<PARK_IN_ORBIT>(new PARK_IN_ORBIT(_fpad));

            action->spk_id    = stof(params["id"]);
            action->R_capture = stof(params["r"]);
            return action;
        } catch (std::exception) {
            LOG_S(ERROR)
                << _fpad.fname << ": Line: " << _fpad.line_no
                << ": Park in orbit needs two elements eg: id:399 r:200";
            return {};
        }
    }

    void operator()(State& state)
    {
        if (obv_idx == -1) {
            obv_idx = orrery::spkid_to_orrery_index(state.orrery, spk_id);
            if (obv_idx == -1) {
                LOG_S(ERROR) << p.line_no << ": park : Can't find " << spk_id
                             << " in Orrery";
                done = true;
                return;
            }
        }

        auto const& ob   = state.orrery[obv_idx];
        auto&       ship = state.ships[p.ship_idx];

        // If this is the first time we call this we need to gather more state
        // information
        if (!last_t_s) {
            last_t_s = ship.state.t;
            return;
        }

        Vector R = ship.state.pos - ob.state.pos;
        if (R.norm() > R_capture) {
            return;
        }

        if (!parking_maneuver_started) {
            DLOG_S(INFO) << p.fname << ": Line " << p.line_no
                         << ": Starting to park " << ship.property.name
                         << " -> " << ob.property.name;
            parking_maneuver_started = true;
        }

        Vector deltaV = parking_deltaV(
            ob.property.GM, ship.state.pos - ob.state.pos, ship.state.vel);

        ship.state.att = deltaV.normed();
        // deltaV = 0.5 * A * Tstep^2
        double Tstep = state.t_s - *last_t_s;
        *last_t_s    = state.t_s;
        double acc   = deltaV.norm() / (0.5 * Tstep * Tstep);

        if (acc > ship.property.max_acc) {
            ship.state.acc = ship.property.max_acc;
            LOG_S(WARNING)
                << p.fname << ": Line " << p.line_no
                << ": acceleration required for parking exceeds max_acc";
        } else {
            ship.state.acc = acc;
        }

        if (deltaV.norm() < epsilon) {
            done = true;
            LOG_S(INFO) << p.fname << ": Line " << p.line_no << ": Ship "
                        << ship.property.name << " successfully parked around "
                        << ob.property.name;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

std::unordered_map<str_t, std::function<fpap_t(const FPAD&, params_t&)>>
    available_actions{ { "set-attitude", SET_ATTITUDE::construct },
                       { "set-accel", SET_ACCEL::construct },
                       { "initial-orbit", INITIAL_ORBIT::construct },
                       { "park", PARK_IN_ORBIT::construct } };

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
            FPAD{ ship_idx, fname, line_no, jd2s(jd) }, params);

    } else {
        LOG_S(ERROR) << fname << ": " << line_no
                     << ": Unknown action: " << tokens[1];
        return {};
    }
}
}