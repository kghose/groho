#include <thread>
#include "simulationmanager.hpp"
using namespace std::chrono_literals;

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sim
{

SimulationManager::SimulationManager( std::string scenario_fname )
: scenario_fname( scenario_fname )
{
  user_command_received = false;
}

void
SimulationManager::run()
{ 
  Scenario scenario( scenario_fname );
  for( ;; )
  {
    std::unique_lock<std::mutex> lk( user_command_mutex );
    cv.wait_for( lk, 500ms );
    // Spurious wake ups are not a problem here
    if( quit_now ) break;

    scenario.load();
    if( scenario.has_changed() )
    { 
      DLOG_S(INFO) << "Rerunning simulation" " << scenario_fname";          
      simulation.rerun_with( scenario );
    }
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