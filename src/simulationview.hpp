/*
  Virtual base class for handling of data mirroring from the simulation

  The display sub-system should sub-class this and DataView and implement 
  the apparatus for mirroring data.
*/
#pragma once

#include "pathview.hpp"


namespace sim
{

class SimulationView
// This class is meant to be derived by display devices
{
public:
  int simulation_version = -1;
  std::string target_frame;

  virtual void initialize( const orrery_body_vec_t&, const space_ship_vec_t& ) = 0;
  // If there is a chance the list of simulation objects could have changed,
  // call this to recreate the views

  virtual PathView& get_path_view_orrery_body( std::string name ) = 0;
  virtual PathView& get_path_view_space_ship( std::string name ) = 0;

  // Return the path view for this object

protected:
  
};

} // namespace sim