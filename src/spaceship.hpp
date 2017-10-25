/*
  Code to manage the "spaceship" elements. These are objects which 
  respond to gravitation from "orrery bodies" (planets, moons, ...)
  but which do not gravitationally affect anything else themselves.  
*/
#include <string>
#include "vector.hpp"

namespace sim
{

class SpaceShip 
{
public:
  enum class 
  FlightState { Landed, Crashed, Flying };
  
  double        max_acceleration,
                max_fuel,
                fuel_consumption_rate;

  LatLon        surface_ll;   // Only valid if ship is landed or crashed               
  double        fuel;
  FlightState   flight_state;
};

}
