#include <chrono>
#include <thread>
#include <iostream>

#include "simulation.hpp"
#include "event.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace config  // some of these may later be read from config files
{

const float threshold = 0.01;

}


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
    if( scenario.requires_recompute() ) { recompute_with( scenario ); }

    std::unique_lock<std::mutex> lk( user_command_mutex );
    cv.wait_for( lk, 500ms ); 
    // Spurious wake ups are not a problem here
    if( quit_now ) break;

    scenario.reload_changes(); 
  }
   
  if( compute_thread.joinable() ) compute_thread.join();  
}


void
Simulation::recompute_with( const Scenario& new_scenario )
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
  mark_sim_buffers_as_ready();

  DLOG_S(INFO) << "Stopping simulation: " << jd;    
}
 
void
Simulation::step( double jd, double dt )
{
  propagate_orrery( jd, dt );
  compute_g();
  propagate_space_fleet( jd, dt );
  resolve_actions( jd );
}

void
Simulation::propagate_orrery( double jd, double dt )
{
  for( auto& b : orrery_bodies ) 
    b.step( jd, dt );
}

void
Simulation::compute_g()
{
  for( auto& b : orrery_bodies ) {
    for( auto& s : space_ships ) {
      Vector R = b.pos - s.pos;
      // Good time to check for collisions too! TODO
      s.accel += (b.GM / R.norm_sq()) * R / R.norm();
    }
  }
}

void 
Simulation::propagate_space_fleet( double jd, double dt )
{
  for( auto& s : space_ships )
  {
    //bool bingo_fuel = false;
    s.update_state( jd, dt);
    // if( bingo_fuel )
    // {
    //   // checkpoints.push_back( checkpointp_t( new Event( 
    //   //   jd,
    //   //   EventType::BingoFuel,
    //   //   v.name,
    //   //   ""
    //   // )));
    // }
  }
}

void
Simulation::resolve_actions( double jd )
{

}

void 
Simulation::mark_sim_buffers_as_ready()
{
  for( auto& b : orrery_bodies ) { b.mark_buffer_as_ready(); }
  for( auto& s : space_ships ) { s.mark_buffer_as_ready(); }
}


void
Simulation::load( Scenario& new_scenario )
{
  copy_mutex.lock();
  // This changes the simulation contents, so we need to lock/unlock
  sim_version_no++;

  // XXX Just for testing
  orrery_bodies = load_debugging_orrery();
  for( auto& b : orrery_bodies ) { b.tolerance_sq = 0.01; }

  copy_mutex.unlock();
}

} // namespace sim
