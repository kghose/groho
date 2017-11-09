#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"
#include "display.hpp"
#include <Fl/Fl.h>


namespace sim
{

Display::Display( SimulationManager& simulation_manager, int width, int height, char* title ) 
: 
Fl_Gl_Window( width, height, title ), simulation_manager( simulation_manager ) 
{
  mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
  mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE | FL_MULTISAMPLE );
  size_range( 400, 400); // This allows resizing. Without this window is fixed size
  /*
  LOG_S(INFO) << "Compiled against GLFW " 
    << GLFW_VERSION_MAJOR << "."
    << GLFW_VERSION_MINOR << "."
    << GLFW_VERSION_REVISION;
  
  int major, minor, revision;
  glfwGetVersion(&major, &minor, &revision);
  LOG_S(INFO) << " Running against GLFW " 
    << major << "."
    << minor << "."
    << revision;

  if (!glfwInit())
  {
    DLOG_S(FATAL) << "GLFW initialization failed";  
    exit(1);
  }
  create_window();
  setup_ui_events();*/
}

Display::~Display()
{
  /*
  glfwDestroyWindow(window);
  glfwTerminate();  
  DLOG_S(INFO) << "Display terminated";
  */
}

void 
Display::create_window()
{
  /*
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  
  window = glfwCreateWindow(800, 600, "Groho ( গ্রহ )", nullptr, nullptr); // Windowed
  //window =
  //    glfwCreateWindow(800, 600, "Groho ( গ্রহ )", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  */
}

// How to neatly pass a member function from https://stackoverflow.com/a/28660673/2512851
void
Display::setup_ui_events()
{
  /*
  glfwSetWindowUserPointer( window, this );
  auto func = [](GLFWwindow* w, int, int, int)
  {
      static_cast<Display*>(glfwGetWindowUserPointer(w))->mouseButtonPressed());
  };
  glfwSetMouseButtonCallback( window, func );
  */
}


void 
Display::run()
{
  Fl::run();
  // while(!glfwWindowShouldClose(window))
  // {
  //   draw();
  //   glfwSwapBuffers(window);
  //   glfwPollEvents();
  // }
}

void 
Display::draw()
{
  /*
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  */
}

} // namespace sim

