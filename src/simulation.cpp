#include <unistd.h>  // temporary will use condition variables instead
#include <iostream>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

#include "simulation.hpp"

namespace sim
{

void 
Simulation::loop()
{
  while( !quit_now )
  {
    restart = false;
    simulation_loop( simulation_parameters );
    if( restart ) { checkpoints.discard_stale_data( simulation_parameters.jd_start ); }
  }
}

void 
Simulation::run( SimulationParameters sp )
{
  simulation_parameters = sp;
  restart = true;
}

void 
Simulation::simulation_loop( SimulationParameters sp )
{
  DLOG_S(INFO) << "Starting simulation: " << sp.jd_start << " - " << sp.jd_end;  

  double jd = sp.jd_start;
  while( !restart & !quit_now & ( jd < sp.jd_end ) ) 
  { 
    //do simulation
    jd += 1;
    std::cout << jd << std::endl;
    sleep(1);
  }
}
  
}