#include "orrerybody.hpp"
#include "units.hpp"


namespace sim
{

orrery_body_vec_t  load_debugging_orrery()
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

  orrery_body_vec_t orrery = {
    {
      "Star",
      "The center of the solar system",
      1,                // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      0.0,              // orbit radius
      0.0               // orbital rate
    },
    {
      "Planet0",
      "A barren wasteland",
      1,                // GM
      1,                // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      1.0_AU,              // orbit radius
      0.1               // orbital rate
    },
    {
      "Planet1",
      "The first warring clan",
      0.1,              // GM
      0.1,              // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      10.0_AU,              // orbit radius
      0.05               // orbital rate
    },
    {
      "Planet2",
      "The second warring clan",
      0.1,              // GM
      0.05,              // radius
      0.0, 0.0, 0.5,    // ra, dec, rotation
      40.0_AU,              // orbit radius
      0.01               // orbital rate
    },    
  };

  return orrery;
}

}