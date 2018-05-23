/*
  Handles units
*/
#pragma once

#include <cmath>

namespace sim {

// Time conversion
const double T0        = 2451545.0; // Julian date for the year 2000
const double S_PER_DAY = 86400.0;

double jd_to_j2000(double jd) { return (jd - T0) * S_PER_DAY; }

double j2000_to_jd(double seconds) { return T0 + seconds / S_PER_DAY; }

// base unit is seconds referred to J2000
inline double operator"" _s(long double x) { return x; }
inline double operator"" _JD(long double jd) { return (jd - T0) * S_PER_DAY; }

// base unit is meters (m)
inline double operator"" _m(long double x) { return x; }
inline double operator"" _km(long double x) { return 1000 * x; }
inline double operator"" _AU(long double x) { return 149597870700 * x; }

// base unit is radians
inline double operator"" _rad(long double x) { return x; }
inline double operator"" _deg(long double x) { return M_PI * x / 180.0; }
}