/*
  Handles the simulation part
*/
#pragma once

#include <atomic>
#include <string>
#include <unistd.h>  // temporary will use condition variables instead


namespace sim
{

class Simulation
{
  std::atomic<bool> quit_now;

public:
  Simulation( std::string scenario_file ) {}
  void quit() { quit_now = true; }
  void loop()
  {
    while( !quit_now )
    {
      usleep(1000);
    }
  }
};

}