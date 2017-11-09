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
  Scenario new_scenario;
  Checkpoints checkpoints;

  Orrery orrery;
  std::unordered_map<std::string, SpaceShip> space_fleet;

public:
  Simulation();

  void run();
  // Main loop. Wait until we are asked to run the simulation, then run

  void rerun_with( const Scenario scenario );
  // Interrupt current simulation and rerun with new scenario
  // The new scenario carries information about what has changed from the
  // existing scenario and we can be smart about what we resimulate.
  // Can be called from different thread


  // void event_loop();
  // wait until we are told to simulate then keep simulating until done
  // or told to resimulate. If done, return to waiting


  void quit();
  // Sets quit_now to tell all simulation related threads to quit

  const Orrery& get_orrery() { return orrery; }
  // Meant for display routines who would like to display the solar system

private:

  void load_orrery( std::string orrery_file );
  // Load the Orrery description file

  void load_flight_plan( std::string flight_plan_file );
  // 
  
  void wait();  
  // Block thread until we need to resimulate

  void simulation_loop( Scenario scenario );
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