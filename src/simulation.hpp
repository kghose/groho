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
#include "orrerybody.hpp"
#include "spaceship.hpp"
#include "simulationview.hpp"


namespace sim
{


class Simulation
{
public:
  Simulation( std::string scenario_fname );

  void run();
  // Main loop. Periodically check for scenario file changes. Rerun sim if needed
  // Respond immediately to external events

  void quit();
  // Sets quit_now to tell all simulation related threads to quit

  void get_view( SimulationView& sv );
  // Return a copy of all trajectories relative to target specified by simulationview
  // reallocate as needed e.g. if simulation has changed

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

  void step( double jd, double dt, double dt2 );
  // Run simulation for one time-step and collect checkpoints as needed

  void leap_frog_1( const double jd, const double dt, const double dt2 );
  // First part of leap-frog algorithm

  void propagate_orrery( const double jd );
  // Update spaceship positions, collect checkpoints as needed

  void update_spaceship_acc();
  // Compute gravitational acceleration on each space ship

  void leap_frog_2( const double dt );
  // Second and final part of leap-frog algorithm

  void resolve_actions( double jd );
  // If there are any actions or interactions, resolve them.
  // Add checkpoints and change spacehsip states as needed

  OrreryBody& get_orrery_body( std::string name );
  // find the relevant orrery body or die


private:
  std::string scenario_fname;  
  // Checkpoints checkpoints;

  orrery_body_vec_t  orrery_bodies;
  space_ship_vec_t   space_ships;


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

  // Stuff needed for the mirroring
  std::atomic<int>   sim_version_no = 0;
  // A counter that we increment each time we re-run. Any one who is mirroring
  // the simulation can check this to see if they need to completely re-copy
  // everything or just do a simple update

  mutable std::mutex copy_mutex;
  // This lock prevents the destruction of the simulation objects during 
  // a simulation restart and prevents mirroring while simulation objects are
  // being destroyed for a simulation restart

  // we need a mechanism by which to prevent view generation when not needed  
};

}