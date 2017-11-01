/*
  Handles the simulation part
*/
#pragma once

#include <atomic>
#include <string>
#include <mutex>
#include <condition_variable>

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
  // Paraphernalia needed for comminucating across threads
  std::atomic<bool>     quit_now  // complete exit
                       , restart  // stop current simulation (if running)
                                  // and rerun with new time range supplied
                                ;

  bool                    action  // some action is required. Used to unblock `loop`
                                ;
  std::mutex m;
  std::condition_variable cv;

  SimulationParameters  simulation_parameters;
  Checkpoints checkpoints;

public:
  Simulation( std::string scenario_file );

  // Block thread until we have something to do
  void wait_for_action();

  void quit();
  
  // wait until we are told to simulate then keep simulating until done
  // or told to resimulate. If done, return to waiting
  void loop();

  // Execute simulation with given parameters
  void run( SimulationParameters sp );
    
  // Run simulation until all time steps are done, or we are asked to quit
  void simulation_loop( SimulationParameters sp );
};

}