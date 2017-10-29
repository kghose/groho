#include <iostream>
#include <stdexcept>
#include <thread>
#include <condition_variable>

#include "simulation.hpp"
#include "display.hpp"
#include "userinterface.hpp"
#include "simplesolarsystem.hpp"

int main(int argc, char* argv[]) 
{

  std::string scenario_file( argv[ 1 ] );
  
  sim::Simulation simulation( scenario_file );
  sim::Display display( simulation );
  sim::UserInterface user_interface( display, simulation );

  std::thread simulation_thread( 
    &sim::Simulation::loop,
    std::ref( simulation )
  );
  
  std::thread display_thread( 
    &sim::Display::loop,
    std::ref( display )
  );

  std::thread user_interface_thread( 
    &sim::UserInterface::loop,
    std::ref( user_interface )
  );
  
  user_interface_thread.join();
  display_thread.join();
  simulation_thread.join();

}

/*
  sim::Orrery orrery;
  orrery.load( scenario_file );

  sim::SpaceShips spaceships;
  spaceships.load( scenario_file );

  sim::SpaceShips spaceships;
  spaceships.load( "dummy ");  

  sim::Simulation simulation( orrery, spaceships );

  float max_error = 1;  // Linear inter
  sim::Checkpoints checkpoints( max_error ) ;
  checkpoints.load( "dummy");

  simulation.set_checkpoints( checkpoints );

  simulation.run( jd_start, jd_stop );  

  checkpoints.save( "dummy");

  for(int i=0; i < 100000; i++) { orrery.propagate( i ); }

  return 0;
}
*/