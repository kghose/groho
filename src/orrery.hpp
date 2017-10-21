/*
   Code to manage the "background" elements - planets, moons, larger asteroids -
   that are sources of gravity and whose orbits are predetermined in some
   manner.

   The Orrery will return an immutable list of objects with positions and
   masses and sizes when asked for the state of the universe at some time t.
 */
#pragma once

#include <cmath>
#include <stdlib.h>
#include <vector>
//#include <string>

#include "vector.hpp"

namespace sim {

/*
   40 AU (Pluto orbit) = 5.984e12 m
   a double will give us 1m resolution with ease
 */
struct BallOfMud 
{
  const std::string name;
  const double GM,          // needed for gravity calculations 
               radius,      // m needed surface calculations (launch/crash)
               ra, dec,     // radians
               rotation;    // radians / day
               // https://en.wikipedia.org/wiki/Axial_tilt#Solar_System_bodies

  Vector pos;

  BallOfMud( 
      std::string _name, 
      double _GM, 
      double _radius, double _ra, double _dec, double _rotation ) 
    : name( _name ), 
      GM( _GM ), 
      radius( _radius ), ra( _ra ), dec( _dec ), rotation( _rotation ) 
  { }

  virtual void compute( double jd ) = 0;

  // Given a surface position in (lat, lon) return the absolute
  // position in the simulation space
  Vector position_in_space( const LatLon &l ) 
  {
    // TODO: Need to figure out meanings of ra, dec
    return Vector();
  }

  // Given a position in space return the projection of that point
  // onto the surface of this ball of mud
  LatLon lat_long( const Vector &v ) 
  {
    // TODO: Need to figure out meanings of ra, dec
    return LatLon();
  }
};

typedef std::vector<std::shared_ptr<BallOfMud>> vect_sptr_bom_t;
// We need a pointer because we will subclass BallOfMud as needed


class Orrery 
{
protected:
  vect_sptr_bom_t body;

public:
  virtual void load( std::string config_file ) = 0;
  virtual const vect_sptr_bom_t& compute( double jd ) = 0;
};

} // namespace sim
