#include <unistd.h>  // temporary will use condition variables instead
#include <iostream>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

#include "simulation.hpp"

namespace sim
{

Simulation::Simulation( std::string scenario_file ) 
{
  action = false;
  quit_now = false;
  restart = false;
}

void 
Simulation::loop()
{
  while( !quit_now )
  {
    wait_for_action();
    if( quit_now ) return;
    if( restart ) 
    { 
      checkpoints.discard_stale_data( simulation_parameters.jd_start );
      simulation_loop( simulation_parameters );
    }
  }
}

void
Simulation::wait_for_action()
{
  std::unique_lock<std::mutex> lk(m);
  cv.wait(lk, [this](){return action;});  // protects against spurious unblocks
  action = false;
}

void
Simulation::quit()
{ 
  std::lock_guard<std::mutex> lk(m);
  action = true;
  quit_now = true;
  cv.notify_one();
}

void 
Simulation::run( SimulationParameters sp )
{
  std::lock_guard<std::mutex> lk(m);
  simulation_parameters = sp;
  action = true;  
  restart = true;
  cv.notify_one();
}

void 
Simulation::simulation_loop( SimulationParameters sp )
{
  restart = false;  
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