#include "orrerybody.hpp"
#include "units.hpp"


namespace sim
{

ob_shptr_vec_t  load_debugging_orrery()
{
  /*
    std::string  name,
    std::string  description,
    double       GM,
    double       radius,
    double       ra,
    double       dec,
    double       rotational_rate, 
    double       orbit_radius,    // for the debugging version
    double       orbit_rate
  */

  ob_shptr_vec_t orrery = {
    ob_shptr( new OrreryBody(
      "Sun",
      "The center of the solar system",
      1.33e20,          // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      1e3,              // orbit radius
      365.0               // orbital rate
    ) ),
    
    ob_shptr( new OrreryBody(
      "Mercury",
      "A barren wasteland",
      2.2e13,           // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      57.91e9,              // orbit radius
      87.96               // orbital rate
    ) ),
    
    ob_shptr( new OrreryBody(
      "Venus",
      "A barren wasteland",
      3.25e14,          // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      108.21e9,              // orbit radius
      224.701               // orbital rate
    ) ),
    
    ob_shptr( new OrreryBody(
      "Earth",
      "A barren wasteland",
      3.99e14,                // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      149.6e9,              // orbit radius
      365.256               // orbital rate
    ) ),
    
    ob_shptr( new OrreryBody(
      "Mars",
      "A barren wasteland",
      4.28e13,                // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      227.92e9,              // orbit radius
      686.98               // orbital rate
    ) ),
    
    ob_shptr( new OrreryBody(
      "Jupiter",
      "A barren wasteland",
      1.267e17,                // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      778.57e9,              // orbit radius
      4332.59               // orbital rate
    ) ),
    
    ob_shptr( new OrreryBody(
      "Saturn",
      "A barren wasteland",
      3.79e16,                // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      1433.53e9,              // orbit radius
      10759.2              // orbital rate
    ) ),
    
    ob_shptr( new OrreryBody(
      "Uranus",
      "A barren wasteland",
      5.79e15,                // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      2872.46e9,              // orbit radius
      30685               // orbital rate
    ) ),
    
    ob_shptr( new OrreryBody(
      "Neptune",
      "The first warring clan",
      6.84e15,              // GM
      0.1,              // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      4495.06e9,              // orbit radius
      60189.0               // orbital rate
    ) ),
    
    ob_shptr( new OrreryBody(
      "Pluto",
      "The second warring clan",
      8.71e11,              // GM
      0.05,              // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      5869.66e9,              // orbit radius
      90465               // orbital rate
    ) ),    
    
    ob_shptr( new OrreryBody(
      "Eris",
      "The second warring clan",
      1.11e12,              // GM
      0.05,              // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      97.651_AU,              // orbit radius
      203830.0               // orbital rate
    ) )        
  };

  return orrery;
}

}