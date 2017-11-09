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
  Vector    camera_pos,
            camera_dir;
  double    fov;
};


/*
struct MouseDrag
{
  bool dragging;
  double  initial_xpos, initial_ypos,
          current_xpos, current_ypos;

  void mouse_button_press( int button, int action, int mods )
  {
    if( button == GLFW_MOUSE_BUTTON_LEFT )
    {
      if ( action == GLFW_RELEASE ) { dragging = false; }
      if ( ( action == GLFW_PRESS ) & ( !dragging ) )
      {
        glfwGetCursorPos( window, &initial_xpos, &initial_ypos );
        dragging = true;
      } 
    } 
  }

  void mouse_move( double xpos, double ypos )
  {
    if( !dragging ) return;
    current_xpos = xpos;
    current_ypos = ypos;
  }

  void 

};
*/

class Display : public Fl_Gl_Window
{
  std::atomic<bool> quit_now;
  SimulationManager& simulation_manager;

  //GLFWwindow* window;

public:
  Display( SimulationManager& simulation_manager, int width, int height, char* title );
  ~Display();

  void create_window();
  // void resize(int x, int y, int w, int h) { Fl_Gl_Window::resize(x, y, w, h); }
  void setup_ui_events();
  void run();
  void draw();

};

}