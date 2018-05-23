/*
*/
#pragma once

#include "units.hpp"
#include "vector.hpp"
#include "event.hpp"
#include "fractaldownsampler.hpp"
#include "nodearray.hpp"


namespace config
{
  using namespace sim;
  const double simulation_scale = 1.0_AU;  
  const size_t vec_buf_size = 10000;
  const size_t evt_buf_size = 100;
  const size_t node_count = 100;
}


namespace sim
{

class SimulationObject
{
public:
  // Common state
  Vector   pos;
  Vector   vel;
  Vector   acc;
  
  // Characteristics
  const std::string name;
  const std::string description;
  const float radius;

  NodeArray<Vector, config::vec_buf_size, config::node_count> path;
  NodeArray<Event,  config::evt_buf_size, config::node_count> events;

public:
  SimulationObject( 
      const std::string name, const std::string description, float radius,
      const Vector& pos = { 0, 0, 0 }, 
      const Vector& vel = { 0, 0, 0 },
      const Vector& acc = { 0, 0, 0 } ) 
      :
      name( name ), description( description ), radius( radius ), 
      pos( pos ), vel( vel ), acc( acc )
  {}

  void append( const Vector& v ) { if( sample( v ) ) path.append( v / config::simulation_scale ); } 
  void append( const Event& e  ) 
  {     
    events.append( e );
    DLOG_S(INFO) << e;
  }
  
private:
  FractalDownsampler  sample;
};

typedef std::shared_ptr<const sim::SimulationObject> cnst_so_shptr;

}



