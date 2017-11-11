#include <random>  /// only for testing XXX
#include <cmath>

#include <Fl/Fl.h>
#include <Fl/Gl.h>
#include <Fl/Glu.h>
#include <Fl/Glut.h>

#include "display.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sim
{

Display::Display( Simulation& simulation, int width, int height, char* title ) 
: 
Fl_Gl_Window( width, height, title ), simulation( simulation ) 
{
  camera.pos = Vector(10, 0, 0);
  camera.dir = Vector(1, 0, 0);
  camera.up = Vector(0, 0, 1);
  camera.fov = 45;
  
  mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE | FL_MULTISAMPLE );
  // FL_MULTISAMPLE = anti-aliasing. WFM.
  size_range( 400, 400); 
  // This allows resizing. Without this window is fixed size


  /* completely dummy testing code take out!!!! */
  std::random_device rd;
  std::mt19937 gen( rd() );
  std::uniform_real_distribution<> u_rand( -50.0, 50.0 );
  std::uniform_real_distribution<> c_rand( 0.2, 1.0 );
  
  dummy_data.clear();
  for(int i = 0; i < 1000; i++)
  {
    dummy_data.push_back(
      std::make_pair(
        Vector( u_rand( gen ), u_rand( gen ), u_rand( gen ) ),
        Vector( c_rand( gen ), c_rand( gen ), c_rand( gen ) )        
      )
    );
  }


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
Display::run()
{
  Fl::run();
}


void
draw_lines()
{
  GLfloat vertices[] =
  {
      1.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 0.0f, 0.0f, 1.0f,
      1.0f, 0.0f, 0.0f, 1.0f
  };

  // This is the identifier for your vertex buffer
GLuint vbo;
// This creates our identifier and puts it in vbo
glGenBuffers(1, &vbo);
// This binds our vbo
glBindBuffer(GL_ARRAY_BUFFER, vbo);
// This hands the vertices into the vbo and to the rendering pipeline    
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// "Enable a port" to the shader pipeline
glEnableVertexAttribArray(0);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
// pass information about how vertex array is composed
glVertexAttribPointer(0, // same as in glEnableVertexAttribArray(0)
                      4, // # of coordinates that build a vertex
                      GL_FLOAT, // data type
                      GL_FALSE, // normalized?
                      0,        // stride
                      (void*)0);// vbo offset

glDrawArrays(GL_LINES, 0, 2);
glDisableVertexAttribArray(0);

}


void 
Display::draw()
{
  static bool firstTime = true;
  if (firstTime)
  {
    glClearColor(.1f, .1f, .1f, 1);
    glEnable(GL_DEPTH_TEST);
    firstTime = false;
  }// if

  // http://www.fltk.org/doc-1.1/Fl_Gl_Window.html
  if (!valid()) 
  {
    glViewport(0,0,w(),h());
  }


  // https://www.opengl.org/archives/resources/faq/technical/viewing.htm
  // Think of the projection matrix as describing the attributes of your camera, 
  // such as field of view, focal length, fish eye lens, etc. Think of the 
  // ModelView matrix as where you stand with the camera and the direction you point it.

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();  
  gluPerspective( camera.fov, (double) w() / (double) h(), .1, 100);


  glMatrixMode(GL_MODELVIEW);      
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      // clear the color and depth buffer  
  glLoadIdentity();  
  Vector scene_center = camera.pos.norm() * camera.dir + camera.pos;  
  gluLookAt( 
    camera.pos.x, camera.pos.y, camera.pos.z, 
    //camera.dir.x, camera.dir.y, camera.dir.z,
    scene_center.x, scene_center.y, scene_center.z, 
    camera.up.x, camera.up.y, camera.up.z);
  for( auto v : dummy_data )
  {
    glPushMatrix();
    glColor3d( v.second.x, v.second.y, v.second.z );
    glTranslatef( v.first.x, v.first.y, v.first.z );
    glutWireSphere(1, 20, 20);                
    glPopMatrix();
  }

}


struct MouseDrag
{
  bool dragging;
  int initial_x, initial_y;
  Camera initial_camera;

  MouseDrag() { dragging = false; }

  void start_drag( int x, int y, Camera camera )
  {
    if( !dragging )
    {
      initial_x = x;
      initial_y = y;
      initial_camera = camera;
    }
  }

  Camera drag( int x, int y )
  {
    Camera  new_camera;
    double  theta = - ( Fl::event_x() - initial_x ) / 500.0,
            phi   = - ( Fl::event_y() - initial_y ) / 500.0;

    new_camera = initial_camera;

    // Yaw
    new_camera.dir = rotate( initial_camera.dir, initial_camera.up, theta );
    // Pitch
    Vector right = cross( initial_camera.dir, initial_camera.up ).normed();
    new_camera.dir = rotate( new_camera.dir, right, phi ).normed();
    new_camera.up = rotate( initial_camera.up, right, phi ).normed();

    return new_camera;
  }

  void end_drag( ) { dragging = false; }
};


int 
Display::handle(int event) {
  static MouseDrag md;

  switch(event) 
  {
    case FL_PUSH:
      //... mouse down event ...
      //... position in Fl::event_x() and Fl::event_y()
      md.start_drag( Fl::event_x(), Fl::event_y(), camera );
      return 1;
    case FL_DRAG:
      //... mouse moved while down event ...
      camera = md.drag( Fl::event_x(), Fl::event_y() );
      redraw();      
      return 1;

    case  FL_MOUSEWHEEL:
      if( FL_COMMAND & Fl::event_state() )
      {
        camera.fov += Fl::event_dy();
      }
      else
      {
        camera.pos = camera.pos + (0.5 * Fl::event_dy() / camera.dir.norm() ) * camera.dir;
      }
      redraw();          
      return 1;

    case FL_RELEASE:   
      //... mouse up event ...
      md.end_drag();    
      return 1;
    case FL_FOCUS :
    case FL_UNFOCUS :
      //... Return 1 if you want keyboard events, 0 otherwise
      return 1;
    case FL_KEYBOARD:
      //... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
      //... Return 1 if you understand/use the keyboard event, 0 otherwise...
      return 1;
    case FL_SHORTCUT:
      //... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
      //... Return 1 if you understand/use the shortcut event, 0 otherwise...
      return 1;
    default:
      // pass other events to the base class...
      return Fl_Gl_Window::handle(event);
  }
}

} // namespace sim

