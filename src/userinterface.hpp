/*
  Handles display part
*/
#pragma once

#include <atomic>
#include <string>
#include <iostream>

#include "simulation.hpp"
#include "display.hpp"

namespace sim
{

class UserInterface
{
  std::atomic<bool> quit_now;
  Display& display;
  Simulation& simulation;

public:
  UserInterface( Display& display, Simulation& simulation ) : 
  display( display ), simulation( simulation ) {}
  void quit() { quit_now = true; }
  void loop()
  {
    while( !quit_now )
    {
      int val = std::cin.get();
      if(val == 'q') {
        simulation.quit();
        display.quit();
        quit_now = true;
      }
      if(val == 'r') {
        simulation.run( SimulationParameters(0, 100, 0.1) );
      }      
    }
  }
};

}