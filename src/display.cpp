#include "display.hpp"

namespace sim
{

Display::Display( Simulation& simulation ) : simulation( simulation )
{
  glfwInit();
  create_window();
}

Display::~Display()
{
  glfwTerminate();  
}

void 
Display::create_window()
{
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  
  window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed
  //window =
  //    glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
  glfwMakeContextCurrent(window);
}
  
} // namespace sim

