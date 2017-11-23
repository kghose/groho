#include "units.hpp" 
#include "spaceship.hpp"


namespace sim
{

space_ship_vec_t 
load_debugging_space_fleet()
{
  space_ship_vec_t fleet = {
    {
      "Durga",
      "A torch-ship",
      1.0,      
      0.0,
      0.0 
    }
  };

  fleet[0].pos = { 1.1_AU, 0, 0.0_AU };
  fleet[0].vel = { 0, 0.0, 20000 };

  return fleet;
}

}

