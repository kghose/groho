#include <chrono>
#include <thread>
#include <iostream>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

#include "simulation.hpp"
#include "event.hpp"

#include "simplesolarsystem.hpp"  // just for testing

namespace sim
{

Simulation::Simulation() 
{
  user_command_received = false;
  quit_now = false;
  restart = false;
}
/*
void 
Simulation::event_loop()
{
  // Best to have this thread here, so that it is naturally destroyed when
  // we exit the loop
  std::thread file_watch_thread( 
    &sim::Simulation::monitor_simulation_files,
    std::ref( *this )
  );
  
  while( !quit_now )
  {
    wait();
    if( quit_now ) break;
    if( restart ) 
    { 
      checkpoints.discard_stale_data( simulation_parameters.jd_start );
      run_simulation( simulation_parameters );
    }
  }

  file_watch_thread.join();  
  // This will have a max delay equal to the file watcher polling period
}

void
Simulation::monitor_simulation_files()
{
  while( !quit_now )
  {
    if( simulation_files_have_changed() )
    {
      std::cerr << "Files changed" << std::endl;
    }
    else
    {
      std::cerr << "Files same" << std::endl;      
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(500ms);
    }  
  }
}
*/
bool
Simulation::simulation_files_have_changed()
{


  return false;
}


void
Simulation::wait()
{
  std::unique_lock<std::mutex> lk( user_command_mutex );
  cv.wait(lk, [this](){ return user_command_received; });  // protects against spurious unblocks
  user_command_received = false;
}

void
Simulation::quit()
{ 
  std::lock_guard<std::mutex> lk( user_command_mutex );
  user_command_received = true;
  quit_now = true;
  cv.notify_one();
}

/*
void 
Simulation::run( SimulationParameters sp )
{
  std::lock_guard<std::mutex> lk( user_command_mutex );
  simulation_parameters = sp;
  user_command_received = true;  
  restart = true;
  cv.notify_one();
}
*/

void 
Simulation::simulation_loop( SimulationParameters sp )
{
  restart = false;  
  DLOG_S(INFO) << "Starting simulation: " << sp.jd_start << " - " << sp.jd_end;  

  double jd = sp.jd_start;
  while( !restart & !quit_now & ( jd < sp.jd_end ) ) 
  { 
    step( jd, sp.dt);
    jd += sp.dt;
    //std::cout << jd << std::endl;
  }

  DLOG_S(INFO) << "Stopping simulation: " << jd;    
}
 
void
Simulation::step( double jd, double dt )
{
  orrery.propagate( jd );
  propagate_space_fleet( jd, dt );
  resolve_actions( jd );
}

void 
Simulation::propagate_space_fleet( double jd, double dt )
{
  for( auto& [k, v] : space_fleet )
  {
    bool bingo_fuel = false;
    v.update_state( dt, orrery.compute_g( v.get_pos() ), bingo_fuel);
    if( bingo_fuel )
    {
      checkpoints.push_back( checkpointp_t( new Event( 
        jd,
        EventType::BingoFuel,
        v.name,
        ""
      )));
    }
  }
}

void
Simulation::resolve_actions( double jd )
{

}

}