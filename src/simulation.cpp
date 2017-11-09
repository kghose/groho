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

void 
Simulation::run()
{  
  while( !quit_now )
  {
    wait();
    if( quit_now ) break;
    if( restart ) 
    { 
      checkpoints.discard_stale_data( new_scenario.start_jd ); // This should go inside loop
      simulation_loop( new_scenario );
    }
  }
}

void
Simulation::rerun_with( const Scenario scenario )
{
  std::lock_guard<std::mutex> lk( user_command_mutex );
  user_command_received = true;
  restart = true;
  new_scenario = scenario;
  cv.notify_one(); 
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


void 
Simulation::simulation_loop( Scenario scenario )
{
  restart = false;  
  DLOG_S(INFO) << "Starting simulation: " << scenario.start_jd << " - " << scenario.stop_jd;  

  double jd = scenario.start_jd;
  while( !restart & !quit_now & ( jd < scenario.stop_jd ) ) 
  { 
    step( jd, scenario.step_jd);
    jd += scenario.step_jd;
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