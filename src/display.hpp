/*
  Handles display part
*/
#pragma once

#include <atomic>
#include <string>
#include <unistd.h>  // temporary will use condition variables instead

#include "simulation.hpp"

namespace sim
{

class Display
{
  std::atomic<bool> quit_now;
  Simulation& simulation;

public:
  Display( Simulation& simulation ) : simulation( simulation ) {}
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