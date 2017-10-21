#include <iostream>

#include "simplesolarsystem.hpp"

int main(int argc, char* argv[]) {
  sim::SimpleSolarSystem sol_sys;
  sol_sys.load( "dummy" );
  for(int i=0; i < 100; i++)
  {
    auto& body = sol_sys.compute( i );
    std::cout << body[1]->pos << std::endl;
  }

  return 0;
}
