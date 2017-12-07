#include <chrono>
#include <thread>
//#include <iostream>
#include <stdexcept>

#include "simulator.hpp"
#include "event.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace config  // some of these may later be read from config files
{

const float threshold = 0.01;

}


namespace sim
{

Simulator::Simulator( std::string _scenario_fname ) 
{
  scenario_fname = _scenario_fname;
  user_command_received = false;
  quit_now = false;
  restart = false;
}

void 
Simulator::run()
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
Simulator::recompute_with( const Scenario& new_scenario )
{
  restart = true;
  if( compute_thread.joinable() ) compute_thread.join();

  // once more unto the breach
  compute_thread = std::thread( 
    &sim::Simulator::compute,
    std::ref( *this ),
    new_scenario
  );
}

void
Simulator::wait()
{
  std::unique_lock<std::mutex> lk( user_command_mutex );
  cv.wait(lk, [this](){ return user_command_received; });  // protects against spurious unblocks
  user_command_received = false;
}

void
Simulator::quit()
{ 
  std::lock_guard<std::mutex> lk( user_command_mutex );
  user_command_received = true;
  quit_now = true;
  cv.notify_one();
}

void 
Simulator::compute( Scenario scenario )
{
  loguru::set_thread_name("compute");
 
  load( scenario );
  
  restart = false;  
  DLOG_S(INFO) << "Starting Simulator: " << scenario.start_jd << " - " << scenario.stop_jd;  

  int simulator_steps = 0;
  double jd = scenario.start_jd;
  while( !restart & !quit_now & ( jd < scenario.stop_jd ) ) 
  { 
    step( jd, scenario.dt );
    jd += scenario.step_jd;
    simulator_steps++;
  }

  DLOG_S(INFO) << "Stopping Simulator: " << jd << " (" << simulator_steps << " steps)";    
}
 
void
Simulator::step( double jd, double dt )
{
  for( auto& s : simulation->space_ships ) { s->update_pos( jd, dt ); }

  for( auto& b : simulation->orrery_bodies ) { b->update_pos( jd ); }

  for( auto& s : simulation->space_ships ) 
  {
    Vector g = { 0, 0, 0 };
    for( auto& b : simulation->orrery_bodies ) {
      Vector R = b->pos - s->pos;
      // Good time to check for collisions too! TODO
      g += (b->GM / R.norm_sq()) * R.normed();
    }
    s->update_acc_and_vel( g, jd, dt );
  }
  resolve_actions( jd );

  simulation->set_last_jd( jd );
}

void
Simulator::resolve_actions( double jd )
{

}

// OrreryBody& 
// Simulator::get_orrery_body( std::string name )
// {
//   for( auto& b : orrery_bodies ) {
//     if( b.name == name ) return b;
//   }
//   std::runtime_error( "No orrery body named" + name );
// }

void
Simulator::load( Scenario& new_scenario )
{
  sim_ptr_t old_sim = simulation;

  simulation = sim_ptr_t( new Simulation );

  // XXX Just for testing
  simulation->orrery_bodies = load_debugging_orrery();
  simulation->space_ships = load_debugging_space_fleet();

  //TODO: 
  if( old_sim != nullptr ) {
    old_sim->mark_as_stale();
  }
}

} // namespace sim
