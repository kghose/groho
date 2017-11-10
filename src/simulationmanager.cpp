#include <thread>
#include "simulationmanager.hpp"
using namespace std::chrono_literals;

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sim
{

SimulationManager::SimulationManager( 
  std::string scenario_fname, Simulation& sim )
: scenario_fname( scenario_fname ), simulation(sim)
{
  user_command_received = false;
}

void
SimulationManager::run()
{ 
  loguru::set_thread_name("sim manager");

  Scenario scenario( scenario_fname );
  for( ;; )
  {
    if( scenario.requires_recompute() ) { simulation.rerun_with( scenario ); }

    std::unique_lock<std::mutex> lk( user_command_mutex );
    cv.wait_for( lk, 500ms );
    // Spurious wake ups are not a problem here
    if( quit_now ) break;

    scenario.reload_changes(); 
  }
}

void
SimulationManager::quit()
{ 
  std::lock_guard<std::mutex> lk( user_command_mutex );
  user_command_received = true;
  quit_now = true;
  cv.notify_one();
}


} // namespace sim