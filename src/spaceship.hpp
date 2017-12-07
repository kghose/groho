/*
  Code to manage the "spaceship" elements. These are objects which 
  respond to gravitation from "orrery bodies" (planets, moons, ...)
  but which do not gravitationally affect anything else themselves.    
*/
#pragma once

#include <string>
#include <vector>
#include "vector.hpp"
#include "simulationobject.hpp"

namespace sim
{

class SpaceShip : public SimulationObject
{  
  enum class FlightState { Landed, Crashed, Flying };

public:
  // Spaceship characteristics
  const double        max_fuel;
  const double        max_acceleration;
  const double        fuel_consumption_rate;

  // Externally visible state
  Vector        attitude = { 0, 0, 0 };
  double        engine_level = 0.0;
  double        fuel = 0.0;

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
      Vector       vel = { 0, 0, 0 },
      double       fuel = 0.0) 
      :
      SimulationObject( name, description, pos, vel ),
      max_fuel( max_f ), max_acceleration( max_a ), 
      fuel_consumption_rate( fcr ), fuel( fuel )
  {}

  void update_pos( double jd, double dt )
  // Do this first
  {
    pos += vel * dt;
    pos.t = jd;
    append( pos );
  }

  void update_acc_and_vel( Vector g, double jd, double dt )
  // Compute g externally, after update_pos and before this
  {
    if( fuel > 0 ) {
      acc = g + ( engine_level * max_acceleration ) * attitude; 
      fuel -= engine_level * fuel_consumption_rate;
      if( fuel <= 0 ) { 
        engine_level = 0;
        append( Event( jd, EventType::BingoFuel, "", "" ) );
      }
    }
    vel += acc * dt;
  }

};

typedef std::shared_ptr<SpaceShip> ss_shptr;
typedef std::vector<ss_shptr>  ss_shptr_vec_t;

ss_shptr_vec_t load_debugging_space_fleet();

} // namespace sim
