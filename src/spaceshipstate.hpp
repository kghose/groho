#pragma once

#include "vector.hpp"
#include "checkpoint.hpp"

namespace sim
{

struct SpaceShipState : public Checkpoint
{
  enum class FlightState { Landed, Crashed, Flying };

  Vector                 pos,
                         vel,
                    attitude;
  double                fuel,
                engine_level;
  FlightState   flight_state;
  LatLon          surface_ll;   // Only valid if ship is landed or crashed               

  SpaceShipState( double jd ) : Checkpoint( jd ) {}

}; //~ 100bytes

}