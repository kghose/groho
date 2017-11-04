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
  sim::Display display( simulation, 400, 400, "গ্রহ" );

  std::thread simulation_thread( 
    &sim::Simulation::loop,
    std::ref( simulation )
  );
    
  display.show();
  display.loop();
  simulation.quit();
  
  simulation_thread.join();
}