#include <random>  /// only for testing XXX
#include <cmath>

#include <Fl/Fl.h>
//#include <Fl/Glu.h>
//#include <Fl/Glut.h>

#include "display.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sim
{

Display::Display( Simulation& simulation, int width, int height, char* title ) 
: 
Fl_Gl_Window( width, height, title ), simulation( simulation ) 
{  
  mode( FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE | FL_MULTISAMPLE );
  // FL_OPENGL3 -> 
  // http://www.fltk.org/doc-1.3/opengl.html ("Using OpenGL 3.0 (or higher versions)")
  // https://github.com/IngwiePhoenix/FLTK/blob/master/examples/OpenGL3-glut-test.cxx
  // FL_MULTISAMPLE -> anti-aliasing. WFM.

  size_range( 400, 400); 
  // This allows resizing. Without this window is fixed size
}

Display::~Display()
{
}

void 
Display::run()
{
  Fl::run();
}


void
Display::setup_opengl()
{
  glClearColor( .1f, .1f, .1f, 1 );
  //glEnable( GL_DEPTH_TEST );
  scene.init();
}


void 
Display::draw()
{
  static bool needs_initialization = true;
  if ( needs_initialization )
  {
    setup_opengl();
    needs_initialization = false;
  }

  // http://www.fltk.org/doc-1.1/Fl_Gl_Window.html
  if ( !valid() ) // something like a resize happened 
  {
    glViewport( 0, 0, w(), h() );
  }

  glClearColor(0, 0, 0, 1); // black
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  scene.render();
}

void
Display::draw_orrery()
{
}

struct MouseDrag
{
  bool dragging;
  int initial_x, initial_y, initial_phi, initial_theta;

  MouseDrag() { dragging = false; }

  void start_drag( int x, int y, sgl::Camera &camera )
  {
    if( !dragging )
    {
      initial_x = x;
      initial_y = y;
      initial_phi = camera.get_phi();
      initial_theta = camera.get_theta();
    }
  }

  void drag( int x, int y, sgl::Camera& camera )
  {
    double  theta = initial_theta - ( Fl::event_x() - initial_x ) / 500.0,
            phi   = initial_phi   - ( Fl::event_y() - initial_y ) / 500.0;

    camera.pan_to( phi, theta );
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
      md.start_drag( Fl::event_x(), Fl::event_y(), scene.camera );
      return 1;
    case FL_DRAG:
      //... mouse moved while down event ...
      md.drag( Fl::event_x(), Fl::event_y(), scene.camera );
      redraw();      
      return 1;

    case  FL_MOUSEWHEEL:
      {
        bool fov_scroll = FL_SHIFT & Fl::event_state(),
             pos_scroll = FL_COMMAND & Fl::event_state(),
             sim_scroll = !fov_scroll & !pos_scroll;

        if( sim_scroll ) {
          // scroll in time
        }

        if( pos_scroll ) {
          scene.camera.dolly_by( 0.5 * Fl::event_dy() );
        }
        
        if( fov_scroll ) {
          scene.camera.change_fov( Fl::event_dy() );
        }
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

