#pragma once

#include <vector>

#include "simulationobject.hpp"


namespace sim
{

class OrreryBody : public SimulationObject
{
public:
  // Body characteristics
  const double    GM;      // needed for gravity calculations 
  const double    radius;  // needed surface calculations (launch/crash)
  
  // Needed for translating surface point to absolute coordinates
  const double    ra;      // RA (radians)
  const double    dec;     // declination (radians)
  const double    rotational_rate;  // rad/ day    
  // https://en.wikipedia.org/wiki/Axial_tilt#Solar_System_bodies

  // This is the debugging version of the OrreryBody and has very simple dynamics
  const double  orbit_radius;  // m
  const double  orbit_rate;    // radians / day
  
  OrreryBody(
      std::string  name,
      std::string  description,
      double       GM,
      double       radius,
      double       ra,
      double       dec,
      double       rotational_rate, 
      double       orbit_radius,    // for the debugging version
      double       orbit_rate
      ) 
      :
      GM( GM ),
      radius( radius ),
      ra( ra ),
      dec( dec ),
      rotational_rate( rotational_rate ),
      orbit_radius( orbit_radius ),
      orbit_rate( orbit_rate ),
      SimulationObject( name, description )
  {}

  // The degugging version
  void 
  update_state( double jd, double dt )
  {
    pos.x = orbit_radius * std::cos( 2.0 * M_PI * orbit_rate * jd );
    pos.y = orbit_radius * std::sin( 2.0 * M_PI * orbit_rate * jd );
    pos.z = 0;
  }

  // Given a surface position in (lat, lon) return the absolute
  // position in the simulation space
  Vector 
  position_in_space( const LatLon &l ) 
  {
    // TODO: Need to figure out meanings of ra, dec
    return Vector();
  }

  // Given a position in space return the projection of that point
  // onto the surface of this ball of mud
  LatLon 
  projected_lat_long( const Vector &v ) 
  {
    // TODO: Need to figure out meanings of ra, dec
    return LatLon();
  }
};


typedef std::vector<OrreryBody>  orrery_body_vec_t;

orrery_body_vec_t  load_debugging_orrery();

} // namespace sim