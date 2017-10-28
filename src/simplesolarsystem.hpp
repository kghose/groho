/*
   This is a Orrery helpful for debugging.
   It's a very simple solar system.
 */
#pragma once

#include "units.hpp"
#include "orrery.hpp"

namespace sim 
{

class SimpleBallOfMud : public BallOfMud
{
public:
  double orbit_radius,  // m
         orbit_rate;    // radians / day

  SimpleBallOfMud(
      std::string  name,
      Vector        pos,      
      double         GM,
      double     radius,
      double         ra,
      double        dec,
      double   rotation,
      double   orbital_rate )
      :
      BallOfMud( name, pos, GM, radius, ra, dec, rotation ),
      orbit_radius( pos.norm() ),
      orbit_rate( orbital_rate )
  {}
               
  void 
  propagate( double jd )
  {
    pos.x = orbit_radius * std::cos( 2.0 * M_PI * orbit_rate * jd );
    pos.y = orbit_radius * std::sin( 2.0 * M_PI * orbit_rate * jd );
    pos.z = 0;
  }

};

typedef std::shared_ptr<SimpleBallOfMud> sptr_sbom_t;

Orrery 
load_simple_solar_system( std::string config_fname );

}  // namespace sim
