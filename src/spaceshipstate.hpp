#pragma once

#include "vector.hpp"

namespace sim
{

class SpaceShipState
{
  enum class FlightState { Landed, Crashed, Flying };

public:
  Vector                 pos,
                         vel,
                    attitude;
  double                fuel,
                engine_level;
  FlightState   flight_state;
  LatLon          surface_ll;   // Only valid if ship is landed or crashed               
}; //~ 100bytes

}