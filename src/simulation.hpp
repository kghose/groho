/*
  Handles the simulation part
*/
#pragma once

#include <atomic>
#include <string>

#include "checkpoint.hpp"

namespace sim
{

struct SimulationParameters
{
  double jd_start,
         jd_end,
         dt;
  
  SimulationParameters() {}
  SimulationParameters( double jd_start, double jd_end, double dt ) 
    : jd_start( jd_start ), jd_end( jd_end ), dt( dt) 
  {}  
};


class Simulation
{
  std::atomic<bool>     quit_now // complete exit
                       , restart // stop current simulation (if running)
                                 // and rerun with new time range supplied
                       ;

  SimulationParameters  simulation_parameters;
  Checkpoints checkpoints;

public:
  Simulation( std::string scenario_file ) 
  {
    quit_now = false;
    restart = false;
  }

  void quit() { quit_now = true; }
  
  // wait until we are told to simulate then keep simulating until done
  // or told to resimulate. If done, return to waiting
  void loop();

  // Execute simulation with given parameters
  void run( SimulationParameters sp );
    
  // Run simulation until all time steps are done, or we are asked to quit
  void simulation_loop( SimulationParameters sp );
};

}