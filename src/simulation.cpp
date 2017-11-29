#include <chrono>
#include <thread>
//#include <iostream>
#include <stdexcept>

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

  int simulation_steps = 0;
  double jd = scenario.start_jd;
  while( !restart & !quit_now & ( jd < scenario.stop_jd ) ) 
  { 
    step( jd, scenario.dt, scenario.dt2 );
    jd += scenario.step_jd;
    simulation_steps++;
  }
  mark_sim_buffers_as_ready();

  DLOG_S(INFO) << "Stopping simulation: " << jd << " (" << simulation_steps << " steps)";    
}
 
void
Simulation::step( double jd, double dt, double dt2 )
{
  copy_mutex.lock();
  leap_frog_1( jd, dt, dt2 );
  propagate_orrery( jd );
  update_spaceship_acc();
  leap_frog_2( dt );
  resolve_actions( jd );
  copy_mutex.unlock();
}

void
Simulation::leap_frog_1( const double jd, const double dt, const double dt2 )
{
  for( auto& s : space_ships ) { s.leap_frog_1( jd, dt, dt2 ); }  
}

void
Simulation::propagate_orrery( const double jd )
{
  for( auto& b : orrery_bodies ) { b.update_pos( jd ); }
}

void
Simulation::update_spaceship_acc()
{
  for( auto& s : space_ships ) {
    Vector g;
    for( auto& b : orrery_bodies ) {
      Vector R = b.pos - s.pos;
      // Good time to check for collisions too! TODO
      g += (b.GM / R.norm_sq()) * R.normed();
    }
    s.compute_acc( g );
  }
}

void
Simulation::leap_frog_2( const double dt )
{
  for( auto& s : space_ships ) { s.leap_frog_2( dt ); }  
}


// void 
// Simulation::propagate_space_fleet( const double jd )
// {
//   for( auto& s : space_ships )
//   {
//     //bool bingo_fuel = false;
//     s.step( jd, dt);
//     // if( bingo_fuel )
//     // {
//     //   // checkpoints.push_back( checkpointp_t( new Event( 
//     //   //   jd,
//     //   //   EventType::BingoFuel,
//     //   //   v.name,
//     //   //   ""
//     //   // )));
//     // }
//   }
// }

void
Simulation::resolve_actions( double jd )
{

}

Path& 
Simulation::get_orrery_body( std::string name )
{
  for( auto& b : orrery_bodies ) {
    if( b.name == target ) return b.path;
  }
  std::runtime_error( "No orrery body named" + name );
}


// TODO: figure out way to stop copying once simulation has ended
void 
Simulation::mirror_data( std::string target, DataMirror& mirror )
{
  auto& new_frame = get_orrery_body( target ); 
  for( auto& b : orrery_bodies ) {
    b.path.transform_to_new_frame( new_frame, mirror.add_orrery_body( b.name ) );
  }
  for( auto& b : space_ships ) {
    b.path.transform_to_new_frame( new_frame, mirror.add_space_ship( b.name ) );
  }
}



// void 
// Simulation::mark_sim_buffers_as_ready()
// {
//   for( auto& b : orrery_bodies ) { b.mark_buffer_as_ready(); }
//   for( auto& s : space_ships ) { s.mark_buffer_as_ready(); }
// }


void
Simulation::load( Scenario& new_scenario )
{
  copy_mutex.lock();
  // This changes the simulation contents, so we need to lock/unlock

  // Safest to explicitly delete 
  orrery_bodies.clear();
  space_ships.clear();

  sim_version_no++;

  // XXX Just for testing
  orrery_bodies = load_debugging_orrery();
  space_ships = load_debugging_space_fleet();

  copy_mutex.unlock();
}

} // namespace sim
