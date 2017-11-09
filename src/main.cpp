#include <iostream>
#include <stdexcept>
#include <thread>
#include <condition_variable>

#include "simulationmanager.hpp"
#include "display.hpp"

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

  sim::Simulation simulation;  
  sim::SimulationManager simulation_manager( scenario_file, simulation );
  sim::Display display( simulation_manager, 400, 400, "গ্রহ" );

  std::thread simulation_manager_thread( 
    &sim::SimulationManager::run,
    std::ref( simulation_manager )
  );
    
  display.show();
  display.run();

  simulation_manager.quit();  
  simulation_manager_thread.join();
}