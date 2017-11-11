/*
  Handles display part
*/
#pragma once

#include <atomic>
#include <string>
#include <thread>

#include <Fl/Fl_Gl_Window.h>

#include "simulationmanager.hpp"

namespace sim
{

struct Camera
{
  Vector    pos,
            dir,
            up;
  double    fov;
};

class Display : public Fl_Gl_Window
{
  std::atomic<bool> quit_now;
  SimulationManager& simulation_manager;
  Camera camera;
  //GLFWwindow* window;

  std::vector<std::pair<Vector, Vector>> dummy_data;
  
public:
  Display( SimulationManager& simulation_manager, int width, int height, char* title );
  ~Display();

  void run();
  void draw();
  int handle( int );
};

}