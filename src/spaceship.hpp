/*
  Code to manage the "spaceship" elements. These are objects which 
  respond to gravitation from "orrery bodies" (planets, moons, ...)
  but which do not gravitationally affect anything else themselves.    
*/
#pragma once

#include <string>
#include <vector>
#include "vector.hpp"
#include "path.hpp"

namespace sim
{

class SpaceShip
{  
  enum class FlightState { Landed, Crashed, Flying };

public:
  // Externally visible state
  Vector        pos      = { 0, 0, 0 };
  Vector        attitude = { 0, 0, 0 };
  double        engine_level = 0.0;
  Path          path;

  // Spaceship characteristics
  const std::string   name;
  const std::string   description;
  const double        max_fuel;
  const double        max_acceleration;
  const double        fuel_consumption_rate;

  FlightState   flight_state;
  LatLon        surface_ll;   // Only valid if ship is landed or crashed

public:
  SpaceShip(
      std::string  name,
      std::string  description,
      double       max_f,      
      double       max_a,
      double       fcr,
      Vector       pos = { 0, 0, 0 },
      Vector       vel = { 0, 0, 0 }) 
      :
      name( name ), description( description ), 
      max_fuel( max_f ), max_acceleration( max_a ), 
      fuel_consumption_rate( fcr ),
      pos( pos ), vel( vel ), path( name, "Solar-System Barycenter" )
  {}

  void 
  leap_frog_1( double jd, double dt, double dt2 )
  // First part of leap frog
  // pos is updated fully, vel is updated partially
  // ** after this, new acc needs to be computed **
  {
    pos += vel * dt + 0.5 * acc * dt2;
    vel += 0.5 * acc * dt;

    pos.t = jd;
    path.add( pos );
  }

  void 
  leap_frog_2( double dt )
  // Second part of leap frog. Call after updating acc
  // vel update is finished 
  {
    vel += 0.5 * acc * dt;
  }

  void 
  compute_acc( Vector g )
  // Add net grav acceleration to engine acceleration 
  // do this inbetween leap_frog_1 and 2
  { 
    _fuel_ran_out = false;
    if( fuel > 0 ) {
      acc = g + ( engine_level * max_acceleration ) * attitude; 
      fuel -= engine_level * fuel_consumption_rate;
      if( fuel <= 0 ) { 
        engine_level = 0;
        _fuel_ran_out = true;
      }
    }
  }

  bool
  fuel_ran_out() { return _fuel_ran_out; }

private:
  // Spaceship state 
  Vector        vel      = { 0, 0, 0 };
  Vector        acc      = { 0, 0, 0 };
  double        fuel     = 1.0;
  bool          _fuel_ran_out = false;
};

typedef std::vector<SpaceShip>  space_ship_vec_t;

space_ship_vec_t load_debugging_space_fleet();

} // namespace sim
