/*
  Handles display part
*/
#pragma once

#include <atomic>
#include <string>
#include <thread>

#include <GLFW/glfw3.h>

#include "simulation.hpp"

namespace sim
{

class Display
{
  std::atomic<bool> quit_now;
  Simulation& simulation;

  GLFWwindow* window;

public:
  Display( Simulation& simulation );
  ~Display();

  void create_window();
  void quit() { glfwSetWindowShouldClose(window, GL_TRUE); }
  void loop()
  {
    while(!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
  }
};

}