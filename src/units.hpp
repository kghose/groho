/*
  Handles units
*/
#pragma once

#include <cmath>

namespace sim
{

// base unit is meters (m)
inline double operator ""  _m( long double x ) { return x; }
inline double operator "" _km( long double x ) { return 1000 * x; }
inline double operator "" _AU( long double x ) { return 149597870700 * x; }

// base unit is radians
inline double operator ""  _rad( long double x ) { return x; }
inline double operator ""  _deg( long double x ) { return M_PI * x / 180.0; }

}