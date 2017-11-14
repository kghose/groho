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

Simulation::Simulation( std::string _scenario_fname ) 
{
  scenario_fname = _scenario_fname;
  user_command_received = false;
  quit_now = false;
  restart = false;
}

void 
Simulation::run()
{  
  using namespace std::chrono_literals;
  
  loguru::set_thread_name("sim manager");
  
  Scenario scenario( scenario_fname );
  for( ;; )
  {
    if( scenario.requires_recompute() ) { rerun_with( scenario ); }

    std::unique_lock<std::mutex> lk( user_command_mutex );
    cv.wait_for( lk, 500ms ); 
    // Spurious wake ups are not a problem here
    if( quit_now ) break;

    scenario.reload_changes(); 
  }
   
  if( compute_thread.joinable() ) compute_thread.join();  
}


void
Simulation::rerun_with( const Scenario& new_scenario )
{
  restart = true;
  if( compute_thread.joinable() ) compute_thread.join();

  // once more unto the breach
  compute_thread = std::thread( 
    &sim::Simulation::compute,
    std::ref( *this ),
    new_scenario
  );
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
Simulation::compute( Scenario scenario )
{
  loguru::set_thread_name("compute");
 
  load( scenario );
  
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
  propagate_orrery( jd, dt );
  propagate_space_fleet( jd, dt );
  resolve_actions( jd );
}

void
Simulation::propagate_orrery( double jd, double dt )
{
  orrery.propagate( jd );  
  for( const auto & [ name, b ] : orrery ) 
  { 
    // XXXX probably need some kind of lock for this
    // orrery_buffer[ name ].add( b->pos ); 
  }  
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

void
Simulation::load( Scenario& new_scenario )
{
  // XXX Just for testing
  orrery = load_simple_solar_system( "" );  
}

} // namespace sim
