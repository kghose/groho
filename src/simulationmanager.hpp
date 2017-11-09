/*
  Loads and parses simulation files, takes care of calling the simulation
  when needed and of managing handoff of results to the display
*/
#pragma once
#include "simulation.hpp"

namespace sim
{

class SimulationManager
{
  std::string scenario_fname;

  Simulation simulation;
  
  // Paraphernalia needed for communicating across threads
  std::atomic<bool>        quit_now  // complete exit
  ;

  bool        user_command_received  // some action is required
  ;
  
  std::mutex     user_command_mutex;
  std::condition_variable        cv;

public:

  SimulationManager( std::string scenario_fname );
  
  void run();
  // Main loop. Wait for user commands and periodically check the 
  // scenario file(s) and rerun simulation as needed

  void quit();
  // Terminate main loop and any running simulations
  // Meant to be called from a different thread

private:
  
};


}
