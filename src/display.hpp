/*
  Handles display part
*/
#pragma once

#include <atomic>
#include <string>
#include <thread>

#include <Fl/Fl_Gl_Window.h>

#include "simulation.hpp"
#include "scene.hpp"


namespace sim
{

class Display : public Fl_Gl_Window
{
public:
  Display( Simulation& simulation, int width, int height, char* title );
  ~Display();

  void run();
  void draw();
  int handle( int );

private:
  void setup_opengl();
  void draw_orrery();

  std::atomic<bool> quit_now;
  Simulation& simulation;
  sgl::Scene scene;
};

}