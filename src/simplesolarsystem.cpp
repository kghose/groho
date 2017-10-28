/*
  This is a Orrery helpful for debugging. 
  It's a very simple solar system.
*/
#include "simplesolarsystem.hpp"

namespace sim
{
  // We kind of totally make this up
  Orrery 
  load_simple_solar_system( std::string config_fname )
  {
    Orrery orrery;

    orrery.add_body( sptr_sbom_t( new SimpleBallOfMud(
        "Star",
        {0, 0, 0},
        1.32712438e20,    // GM
        7e8,              // radius
        0.0, 0.0, 0.5,    // ra, dec, rotation
        0.0               // orbital rate
    ) ) );

    orrery.add_body( sptr_sbom_t( new SimpleBallOfMud(
        "Planet1",         // name
        {1.0_AU, 0, 0},
        0.3986e15,         // GM
        6.4e6,             // radius
        0.0, 0.0, 6.2832,  // ra, dec, rotation
        1e-3               // orbital rate rad/day
    ) ) );            
    
    orrery.add_body( sptr_sbom_t( new SimpleBallOfMud(
        "Planet2",         // name
        {2.0_AU, 0, 0},
        0.04e15,           // GM
        3.4e6,             // radius
        0.0, 0.0, 6.2832,  // ra, dec, rotation
        0.5e-3             // orbital rate
    ) ) );

    return orrery;            
  }
}
