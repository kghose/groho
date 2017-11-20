/*
  Nicely packages the simulation objects so that they can be simulated,
  checkpointed and mirrored
*/
#pragma once

#include <unordered_map>
#include <memory>
#include "simulationbuffer.hpp"


namespace sim
{

class SimulationObject
// We derive this class for different classes of simulation objects
{
public:
  std::string         name;

  SimulationBuffer    simulation_buffer;
  float               tolerance_sq;  // The simulation can adjust this

  virtual void update_state( double jd, double dt, const Vector& g ) = 0;
  // this is called every simulation step

};

typedef std::unordered_map<std::string, std::unique_ptr<SimulationObject>>  simobj_map_t;

}

