#include <iostream>
#include <stdexcept>
#include <thread>
#include <condition_variable>

#include "simulation.hpp"
#include "display.hpp"
#include "userinterface.hpp"

#define LOGURU_IMPLEMENTATION 1
#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


void print_usage()
{
  std::cout << "\nUsage:\n" <<  
  "groho <scenario file>" << std::endl;
}


int main(int argc, char* argv[]) 
{
  if( argc < 2 ) 
  {
    print_usage();
    exit(0);
  }

  loguru::init(argc, argv);

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