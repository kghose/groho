/*
   This is a Orrery helpful for debugging.
   It's a very simple solar system.
 */
#pragma once

#include "orrery.hpp"

namespace sim 
{

struct SimpleBallOfMud : public BallOfMud
{
  const 
  double orbital_radius,  // m
         orbital_rate;   // radians / day

  SimpleBallOfMud( 
      std::string _name, 
      double _GM, 
      double _radius, double _ra, double _dec, double _rotation, 
      double orb_rad, double orb_rate) 
  : 
  BallOfMud(
    _name, 
    _GM, 
    _radius, _ra, _dec, _rotation
  ) , 
  orbital_radius( orb_rad ), orbital_rate( orb_rate ) { ; }

  void compute( double jd );

};

typedef std::shared_ptr<SimpleBallOfMud> sptr_sbom_t;


class SimpleSolarSystem : public Orrery 
{

public:
  void load( std::string config_file );
  const vect_sptr_bom_t& compute( double tdb );

};

}  // namespace sim
