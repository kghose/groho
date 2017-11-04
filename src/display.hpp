/*
  Handles display part
*/
#pragma once

#include <atomic>
#include <string>
#include <thread>

//#include <GLFW/glfw3.h>
#include <Fl/Fl_Gl_Window.h>

#include "simulation.hpp"

namespace sim
{

struct DisplayState
// Neatly collects what we want to display and how we change that via
// user interactions into one data structure
{
  Vector    camera_pos,
            camera_dir;

  
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
  Simulation& simulation;

  //GLFWwindow* window;

public:
  Display( Simulation& simulation, int width, int height, char* title );
  ~Display();

  void create_window();
  void setup_ui_events();

  void quit() { /*glfwSetWindowShouldClose(window, GL_TRUE);*/ }
  void loop();
  void draw();

};

}