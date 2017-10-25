/*
  Base class for simulation objects
*/
#pragma once 

#include <cmath>
#include <iostream>

#include "vector.hpp"

namespace sim 
{

/*
  There are two objects in the simulation: BallOfMuds (Planets/moons) 
  and Spaceships. They have different properties and behaviors in the
  simulation, but both of them have the following properties in common.
  These are also the properties that determine the frequency of saving
  checkpoints.  
*/
class Body
{
public:
  std::string   name;
  Vector        pos,
                vel,
                accel;
  Body( 
      std::string  name)
      :
      name( name )
  {}
              
  Body( 
      std::string  name,
      Vector        pos,
      Vector        vel,
      Vector      accel)
      :
      name( name ),
      pos( pos ),
      vel( vel ),
      accel( accel )
  {}
};
/*
  Why we use double, rather than float for position:
    We would like 1m resolution
    40 AU (Pluto orbit) = 5.984e12 m
    a float will cap out at 2**24 = 16777216 m for 1 m resolution 
    a double will give us 1m resolution with ease at this range  
*/


} // namespace sim