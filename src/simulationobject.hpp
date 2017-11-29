// DEPRECATED


/*
  SimulationObject carries the core state and history common for all simulation
  objects. Specialized objects derive from this class adding functions for state
  updation and carrying specialized private state that is not needed for display.
*/
#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include "simulationbuffer.hpp"


namespace sim
{

class SimulationObject
// We derive this for different simulation objects
{
public:
  // Current state (common across all objects)
  Vector   pos;       
  Vector   vel;       

  // State history
  SimulationBuffer    simulation_buffer;
  float               tolerance_sq;  // The simulation can adjust this

  // Static metadata
  const std::string   name;
  const std::string   description;

public:
  SimulationObject( std::string name, std::string description ) :
      name( name ), description( description )
  {}

  void step( double jd, double dt )
  // this is called every simulation step
  {
    update_state( jd, dt );
    simulation_buffer.add( jd, pos );
  }

  void mark_buffer_as_ready()
  {
    simulation_buffer.finalize();
  }

protected:
  virtual void update_state( double jd, double dt ) = 0;
};

typedef std::unordered_map<std::string, std::shared_ptr<SimulationObject>>  simobj_map_t;
typedef std::vector<std::shared_ptr<SimulationObject>>                      simobj_vec_t;

}

