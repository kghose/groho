/*
  Code to manage the "spaceship" elements. These are objects which 
  respond to gravitation from "orrery bodies" (planets, moons, ...)
  but which do not gravitationally affect anything else themselves.    
*/
#pragma once

#include <string>
#include <vector>
#include "vector.hpp"
#include "spaceshipstate.hpp"

namespace sim
{

class SpaceShip
{
public:

  std::string             name;

  double              max_fuel,  
              max_acceleration,
         fuel_consumption_rate;

  Vector                 accel;    // net acceleration ( engine + gravity )
  SpaceShipState         state;

  bool              bingo_fuel;

  SpaceShip(
      std::string  name,
      double      max_f,      
      double      max_a,
      double        fcr,
      SpaceShipState ss)
      :
      name( name ),
      max_fuel( max_f ),
      max_acceleration( max_a ),
      fuel_consumption_rate( fcr ),
      state( ss )
  {}

  void
  set_attitude( const Vector& v )
  {
    state.attitude = v;
  }
  
  const Vector& 
  get_pos() { return state.pos; }
  // This is needed by the simulator to compute g (accel due to gravity)

  const SpaceShipState
  get_state() { return state; }
  // This is intended to fetch the state struct once we want to save a checkpoint

  bool
  bingo_fuel_event()
  {
    if( bingo_fuel ) {
      bingo_fuel = false;
      return true;
    }
    else
    {
      return false;      
    }
  }

  void
  update_state( double dt, const Vector& g )
  // update position and velocity and set flag for bingo fuel if needed 
  {    
    state.pos += dt * state.vel;
    state.vel += dt * accel;

    if( state.fuel > 0 )
    {
      state.fuel -= state.engine_level * fuel_consumption_rate;
      if( state.fuel <= 0 ) 
      { 
        state.engine_level = 0;
        bingo_fuel = true;
      }
    }

    accel = (( state.engine_level * max_acceleration ) * state.attitude) + g; 
    
  }
};

} // namespace sim
