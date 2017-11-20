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
  
  void draw();                
  
  int handle( int );       
  // User event handling

  static void mirror_simulation( void* );          
  // Called periodically to poll simulation for fresh data


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