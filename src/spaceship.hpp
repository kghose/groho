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
  const double  max_fuel;
  const double  max_acceleration;
  const double  fuel_consumption_rate;

  // Spaceship specific state 
  Vector        attitude;
  double        fuel;
  double        engine_level;
  Vector        accel;  // We accumulate total acceleration here (engine + gravity)
                        // and apply it when we update the position

  FlightState   flight_state;
  LatLon        surface_ll;   // Only valid if ship is landed or crashed               

  bool          fuel_event = false;

  SpaceShip(
      std::string  name,
      std::string  description,
      double       max_f,      
      double       max_a,
      double       fcr) 
      :
      max_fuel( max_f ),
      max_acceleration( max_a ),
      fuel_consumption_rate( fcr ),
      SimulationObject( name, description )
  {}

  void
  update_state( double jd, double dt )
  // update position and velocity and set flag for bingo fuel if needed 
  { 
    // at this point we've computed acceleration due to gravity
    // and placed it in accel
    fuel_event = false;
    if( fuel > 0 ) {
      accel += ( engine_level * max_acceleration ) * attitude; 
      fuel -= engine_level * fuel_consumption_rate;
      if( fuel <= 0 ) { 
        engine_level = 0;
        fuel_event = true;
      }
    }
    pos += dt * vel;
    vel += dt * accel;
    accel = {0, 0, 0};
  }
};

typedef std::vector<SpaceShip>  space_ship_vec_t;

} // namespace sim
