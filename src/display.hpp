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
  // Main FLTK loop
  
  void refresh_simulation_data();          
  // Runs in separate thread, periodically polls simulation for fresh data
  
  void draw();                
  
  int handle( int );       
  // User event handling

private:
  
  void setup_opengl();
  // Misc OpenGL initialization

  void draw_orrery();

private:
  std::atomic<bool> quit_now;
  Simulation& simulation;
  sgl::Scene scene;
};

}