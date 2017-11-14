/*
  Handles the simulation part
*/
#pragma once

#include <atomic>
#include <string>
#include <mutex>
#include <condition_variable>

#include "scenario.hpp"
#include "checkpoint.hpp"
#include "orrery.hpp"
#include "spaceship.hpp"


namespace sim
{


class Simulation
{
  std::string scenario_fname;  
  Checkpoints checkpoints;

  Orrery orrery;
  std::unordered_map<std::string, SpaceShip> space_fleet;
  
  std::thread compute_thread;  // The simulation loop runs in yet another thread

  // Paraphernalia needed for comminucating across threads
  std::atomic<bool>     quit_now  // complete exit
                       , restart  // stop current simulation (if running)
                                  // and rerun with new time range supplied
  ;

  bool     user_command_received  // some action is required. Used to unblock `loop`
                                ;
  std::mutex user_command_mutex;
  std::condition_variable cv;

public:
  Simulation( std::string scenario_fname );

  void run();
  // Main loop. Periodically check for scenario file changes. Rerun sim if needed
  // Respond immediately to external events

  void rerun_with( const Scenario& scenario );
  // Interrupt current simulation and rerun with new scenario
  // The new scenario carries information about what has changed from the
  // existing scenario and we can be smart about what we resimulate.
  // Called from same thread as "run". 
  // This will block until compute_loop can exit i.e. one simulation time step

  void quit();
  // Sets quit_now to tell all simulation related threads to quit

  const cdbuf_map_t& get_orrery_buffer() { return orrery_buffer; }

private:

  void compute( Scenario scenario );
  // Do computations until all time steps are done, we need to restart or quit
  
  void load( Scenario& new_scenario );
  // Load scenario files

  void load_orrery( std::string orrery_file );
  // Load the Orrery description file

  void load_flight_plan( std::string flight_plan_file );
  // 
  
  void wait();  
  // Block thread until we need to resimulate

  void step( double jd, double dt );
  // Run simulation for one time-step and collect checkpoints as needed

  void propagate_orrery( double jd, double dt );
  // Update spaceship positions, collect checkpoints as needed

  void propagate_space_fleet( double jd, double dt );
  // Update spaceship positions, collect checkpoints as needed

  void resolve_actions( double jd );
  // If there are any actions or interactions, resolve them.
  // Add checkpoints and change spacehsip states as needed
};

}