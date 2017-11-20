/*
  The Heinlen takes care of updating spaceships just like Orrery takes care
  of updating the planets.
*/
#pragma once

#include <unordered_map>
#include <string>
#include "spaceship.hpp"


namespace sim
{

class Heinlein : public std::unordered_map<std::string, SpaceShip>
{

public:
  
  void 
  set_spaceship_state( std::string name, SpaceShipState state )
  {
    (*this)[ name ].set_state( state );
  }

  void 
  update( double jd, );


};

} // namespace sim


