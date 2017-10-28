#include <iostream>
#include <stdexcept>

#include "simplesolarsystem.hpp"

int main(int argc, char* argv[]) {

  sim::Orrery orrery;
  orrery.load( sim::load_simple_solar_system( "dummy" ) );
/*
  sim::SpaceShips spaceships;
  spaceships.load( "dummy ");  

  sim::Simulation simulation( orrery, spaceships );

  float max_error = 1;  // Linear inter
  sim::Checkpoints checkpoints( max_error ) ;
  checkpoints.load( "dummy");

  simulation.set_checkpoints( checkpoints );

  simulation.run( jd_start, jd_stop );  

  checkpoints.save( "dummy");
*/

  for(int i=0; i < 100000; i++)
  {
    orrery.propagate( i );
    for(int j = 0; j < orrery.size(); j++)
    {
      std::cout << orrery[ j ]->pos;
      if( j < orrery.size() - 1) std::cout << ", ";  
    }
    std::cout << std::endl;
  }

  return 0;
}
