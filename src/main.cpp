#include <iostream>
#include <stdexcept>
#include <thread>
#include <condition_variable>

#include "simulator.hpp"
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

  sim::Simulator simulator( scenario_file );  
  sim::Display display( simulator, 400, 400, "গ্রহ" );

  std::thread simulator_thread( 
    &sim::Simulator::run,
    std::ref( simulator )
  );
    
  display.show();
  display.run();

  simulator.quit();  
  simulator_thread.join();
  
}