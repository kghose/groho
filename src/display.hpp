/*
  Handles display part
*/
#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <vector>

#include <Fl/Fl_Gl_Window.h>

#include "simulator.hpp"
#include "camera.hpp"
#include "trajectory.hpp"


namespace sim
{

class Display : public Fl_Gl_Window
{
public:
  Display( Simulator& simulator, int width, int height, char* title );
  ~Display();

  void run();
  // Main FLTK loop
  
  void draw();                
  // Called by FLTK

  void render_simulation();
  // draw the orrery and spaceships

  int handle( int );       
  // User event handling

  static void refresh_simulation( void* );          
  // Called periodically to see if we need to redraw the simulation or reload it
  // only static functions can be passed to the callback

private:
  
  void setup_opengl();
  // Misc OpenGL initialization

  void initialize_shaders();
  //   

  void load_simulation();
  // Create a new simulation_objects list from scratch 

  void view_simulation();
  // geenrate a view of the simulation into existing buffers

private:
  std::atomic<bool> quit_now;
  Simulator& simulator;  
  const_sim_ptr_t simulation;
  float last_display_jd = 0;
  // We use this to decide whether we need to copy the simulation data to the
  // OpenGL buffers

  sgl::Camera camera;
  sgl::ShaderProgram shader_program;
  sgl::traj_ptr_vect_t simulation_objects; 

  //sgl::Scene scene;
};

}