/*
  This class stores all state and history for a simulation and is meant to
  be shared between the Simulator and Display. The Simulator has write access
  and the Display has read access. A Simulation can become detached from the
  Simulator if a new Simulation is started but the Display can still access it
*/
#pragma once

#include <memory>

#include "orrerybody.hpp"
#include "spaceship.hpp"


namespace sim
{

struct Simulation
{
  ob_shptr_vec_t  orrery_bodies;
  ss_shptr_vec_t   space_ships;

  void mark_as_stale()  { stale.store( true, std::memory_order_release ); }
  bool is_stale() const { return stale.load( std::memory_order_acquire ); }

  void  set_last_jd( float jd ) { last_jd.store( jd, std::memory_order_release ); }
  float get_last_jd() const     { return last_jd.load( std::memory_order_acquire ); }

private:
  std::atomic<bool>   stale = false;
  // The simulator has moved on to a new simulation

  std::atomic<float>  last_jd = 0;
  // Last time stamp Simulator computed
  // This can be used by the Display to decide if a refresh of the view is warranted
};

typedef std::shared_ptr<Simulation> sim_ptr_t;
typedef std::shared_ptr<const Simulation> const_sim_ptr_t;


} //namespace sim
