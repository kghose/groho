/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Convenience functions for unit conversion when loading from data files.
Internally SI units are used. For time, the internal units are J2000 seconds
because that is what the JPL SPK files use
*/
#pragma once

#include <cmath>
#include <ostream>

namespace groho {

// Time conversion
constexpr double GREGORIAN_EPOCH = 1721425.5;
constexpr double T0              = 2451545.0; // Julian date for the year 2000
constexpr double S_PER_DAY       = 86400.0;

struct GregorianDate {
    int    Y;
    int    M;
    int    D;
    double H;
};

std::ostream& operator<<(std::ostream& os, const GregorianDate& cd)
{
    os << cd.Y << "." << cd.M << "." << cd.D << "." << cd.H;
    return os;
}

bool leap_gregorian(int year)
{
    return ((year % 4) == 0) && (!(((year % 100) == 0) && ((year % 400) != 0)));
}

class JulianDate {

public:
    JulianDate(const double jd)
        : jd(jd)
    {
    }

    JulianDate(const GregorianDate& cd)
    {
        // https://www.fourmilab.ch/documents/calendar/calendar.js
        double jdn = (GREGORIAN_EPOCH - 1) + (365 * (cd.Y - 1))
            + ((cd.Y - 1) / 4) + (-((cd.Y - 1) / 100)) + ((cd.Y - 1) / 400)
            + ((((367 * cd.M) - 362) / 12)
               + ((cd.M <= 2) ? 0 : (leap_gregorian(cd.Y) ? -1 : -2)) + cd.D);

        jd = jdn + cd.H;

        /*
        //https://en.wikipedia.org/wiki/Julian_day#Julian_day_number_calculation

        int jdn = (1461 * (cd.Y + 4800 + (cd.M - 14) / 12)) / 4
            + (367 * (cd.M - 2 - 12 * ((cd.M - 14) / 12))) / 12
            - (3 * ((cd.Y + 4900 + (cd.M - 14) / 12) / 100)) / 4 + cd.D - 32075;
        jd = jdn + cd.H;
        */
    }

    constexpr operator double() const { return jd; }

    GregorianDate as_ut() const
    {
        // https://www.fourmilab.ch/documents/calendar/calendar.js
        // https://www.fourmilab.ch/documents/calendar/astro.js
        double wjd        = int(jd - 0.5) + 0.5;
        int    depoch     = wjd - GREGORIAN_EPOCH;
        int    quadricent = depoch / 146097;
        int    dqc        = depoch % 146097;
        int    cent       = dqc / 36524;
        int    dcent      = dqc % 36524;
        int    quad       = dcent / 1461;
        int    dquad      = dcent % 1461;
        int    yindex     = dquad / 365;
        int    year = (quadricent * 400) + (cent * 100) + (quad * 4) + yindex;
        if (!((cent == 4) || (yindex == 4))) {
            year++;
        }
        int yearday = wjd - JulianDate(GregorianDate{ year, 1, 1, 0 });
        int leapadj
            = ((wjd < JulianDate(GregorianDate{ year, 3, 1, 0 }))
                   ? 0
                   : (leap_gregorian(year) ? 1 : 2));
        int month = ((((yearday + leapadj) * 12) + 373) / 367);
        int day   = (wjd - JulianDate(GregorianDate{ year, month, 1, 0 })) + 1;

        double cjd  = jd + 0.5;
        double hour = cjd - int(cjd);

        return GregorianDate{ year, month, day, hour };

        /*
        //https://en.wikipedia.org/wiki/Julian_day#Julian_day_number_calculation

        int J = int(jd); // 0hr = 12 Noon UT

        int y = 4716, j = 1401, m = 2, n = 12, r = 4, p = 1461, v = 3, u = 5,
            s = 153, w = 2, B = 274277, C = -38;

        int    f = J + j + (((4 * J + B) / 146097) * 3) / 4 + C;
        int    e = r * f + v;
        int    g = (e % p) / r;
        int    h = u * g + w;
        int    D = (h % s) / u + 1;
        int    M = (h / s + m) % n + 1;
        int    Y = e / p - y + (n + m - M) / n;
        double H = jd + 0.5 - J;

        return GregorianDate{ Y, M, D, H };
        */
    }

private:
    double jd;
};

// We handle the SPK J2000 frame which is seconds since J2000
// (SPK frame_id == 1)
class J2000_s {
public:
    J2000_s(const double j) { j2000_s = j; }
    J2000_s(const GregorianDate& cd) { j2000_s = J2000_s(JulianDate(cd)); }
    J2000_s(const JulianDate& jd) { j2000_s = (jd - T0) * S_PER_DAY; }

    JulianDate as_jd() const
    {
        return JulianDate{ (j2000_s / S_PER_DAY) + T0 };
    }

    GregorianDate as_ut() const { return as_jd().as_ut(); }

    constexpr operator double() { return j2000_s; }

private:
    double j2000_s;
};

double from_km(double km) { return 1000 * km; }
double from_AU(double au) { return 149597870700 * au; }

double operator""_km(long double km) { return from_km(km); }
double operator""_AU(long double au) { return from_AU(au); }
}