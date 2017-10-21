/*
  This is a Orrery helpful for debugging. 
  It's a very simple solar system.
*/
#include "simplesolarsystem.hpp"

namespace sim
{
  void SimpleBallOfMud::compute( double jd )
  {
    pos.x = orbital_radius * std::cos( 2.0 * M_PI * orbital_rate * jd );
    pos.y = orbital_radius * std::sin( 2.0 * M_PI * orbital_rate * jd );
    pos.z = 0;
  }


  // We kind of totally make this up
  void SimpleSolarSystem::load( std::string config_file )
  {
    body.push_back(
      sptr_sbom_t( 
        new SimpleBallOfMud(
          "Star",           // name
          1.32712438e20,    // GM
          7e8,              // radius
          0.0, 0.0, 0.5,    // ra, dec, rotation
          0.0,              // orbital radius
          0.0               // orbital rate
        )
      )
    );            

    body.push_back( 
      sptr_sbom_t(         
        new SimpleBallOfMud(
          "Planet1",        // name
          0.3986e15,        // GM
          6.4e6,            // radius
          0.0, 0.0, 6.2832, // ra, dec, rotation
          1.5e11,           // orbital radius
          1e-3               // orbital rate
    )));            
    
    body.push_back(
      sptr_sbom_t(         
        new SimpleBallOfMud(
          "Planet2",        // name
          0.04e15,          // GM
          3.4e6,            // radius
          0.0, 0.0, 6.2832, // ra, dec, rotation
          3.0e11,           // orbital radius
          0.5e-3               // orbital rate
    )));            
  }

  const vect_sptr_bom_t& SimpleSolarSystem::compute( double jd )
  {
    for(int i = 1 ; i < body.size(); i++ ) { body[ i ]->compute( jd ); }
    return body;
  }

}
