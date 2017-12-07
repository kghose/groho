#include "units.hpp" 
#include "spaceship.hpp"


namespace sim
{

ss_shptr_vec_t 
load_debugging_space_fleet()
{
  ss_shptr_vec_t fleet = {
    ss_shptr( new SpaceShip(
      "Durga",
      "A torch-ship",
      1.0,      
      0.1,
      1e-2,
      { 1.1_AU, 0, 0.0_AU },
      { 0, 20000.0, 0 }
    ) ),
  };

  fleet[0]->attitude = { 0, 1.0, 0.0 };
  fleet[0]->engine_level = 1;

  return fleet;
}

}

