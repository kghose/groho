/*
  Handles display part
*/
#pragma once

#include <atomic>
#include <string>
#include <thread>

#include <Fl/Fl_Gl_Window.h>

#include "simulation.hpp"

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
  Simulation& simulation;
  Camera camera;

  std::vector<std::pair<Vector, Vector>> dummy_data;
  
public:
  Display( Simulation& simulation, int width, int height, char* title );
  ~Display();

  void run();
  void draw();
  int handle( int );

private:
  void draw_orrery();
};

}