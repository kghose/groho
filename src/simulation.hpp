/*
  Handles the simulation part
*/
#pragma once

#include <atomic>
#include <string>
#include <mutex>
#include <condition_variable>

#include "checkpoint.hpp"
#include "orrery.hpp"
#include "spaceship.hpp"


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

  bool     user_command_received  // some action is required. Used to unblock `loop`
                                ;
  std::mutex user_command_mutex;
  std::condition_variable cv;

  SimulationParameters  simulation_parameters;
  Checkpoints checkpoints;

  Orrery orrery;
  std::unordered_map<std::string, SpaceShip> space_fleet;

public:
  Simulation( std::string scenario_file );

  void quit();
  
  void loop();
  // wait until we are told to simulate then keep simulating until done
  // or told to resimulate. If done, return to waiting

  void run( SimulationParameters sp );
  // Execute simulation with given parameters

  const Orrery& get_orrery() { return orrery; }
  // Meant for display routines who would like to display the solar system

private:

  void wait_for_user_command();  
  // Block thread until we have something to do

  void simulation_loop( SimulationParameters sp );
  // Run simulation until all time steps are done, or we are asked to quit

  void step( double jd, double dt );
  // Run simulation for one time-step and collect checkpoints as needed

  void propagate_space_fleet( double jd, double dt );
  // Update spaceship positions, collect checkpoints as needed

  void resolve_actions( double jd );
  // If there are any actions or interactions, resolve them.
  // Add checkpoints and change spacehsip states as needed
};

}