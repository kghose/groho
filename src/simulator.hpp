/*
  Handles the simulation part
*/
#pragma once

#include <string>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "scenario.hpp"
#include "simulation.hpp"
// #include "orrerybody.hpp"
// #include "spaceship.hpp"
// #include "simulationview.hpp"


namespace sim
{

class Simulator
{
public:
  Simulator( std::string scenario_fname );

  void run();
  // Main loop. Periodically check for scenario file changes. Rerun sim if needed
  // Respond immediately to external events

  void quit();
  // Sets quit_now to tell all simulation related threads to quit

  const_sim_ptr_t get_simulation() const { return simulation; }
  // Return us a pointer to the current simulation being run
  // We typically call this after discovering we are holding a stale
  // simulation

private:
  void recompute_with( const Scenario& scenario );
  // Interrupt current simulation and rerun with new scenario
  // The new scenario carries information about what has changed from the
  // existing scenario and we can be smart about what we resimulate.
  // Called from same thread as "run". 
  // This will block until compute_loop can exit i.e. one simulation time step

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

  void resolve_actions( double jd );
  // If there are any actions or interactions, resolve them.
  // Add checkpoints and change spacehsip states as needed

  // OrreryBody& get_orrery_body( std::string name );
  // find the relevant orrery body or die


private:
  std::string scenario_fname;  
  // Checkpoints checkpoints;

  sim_ptr_t   simulation = nullptr;
  // The simulation storing all the state and history


  std::thread compute_thread;  // The simulation loop runs in yet another thread

  // Paraphernalia needed for comminucating across threads
  std::atomic<bool>     quit_now  // complete exit
                       , restart  // stop current simulation (if running)
                                  // and rerun with new time range supplied
  ;

  bool     user_command_received  // some action is required. Used to unblock `loop`
                                ;
  std::mutex                user_command_mutex;
  std::condition_variable   cv;

  mutable std::mutex copy_mutex;
  // This lock prevents the destruction of the simulation objects during 
  // a simulation restart and prevents mirroring while simulation objects are
  // being destroyed for a simulation restart

  // we need a mechanism by which to prevent view generation when not needed  
};

}