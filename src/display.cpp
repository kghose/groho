#include <random>  /// only for testing XXX
#include <cmath>

#include <Fl/Fl.h>
#include <Fl/gl.h>

#include "display.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace config
{
  const float sim_poll_interval = 0.1;
}


namespace sim
{

Display::Display( Simulator& simulator, int width, int height, char* title ) : 
    Fl_Gl_Window( width, height, title ), simulator( simulator ) 
{  
  mode( FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE | FL_MULTISAMPLE );
  // FL_OPENGL3 -> 
  // http://www.fltk.org/doc-1.3/opengl.html ("Using OpenGL 3.0 (or higher versions)")
  // https://github.com/IngwiePhoenix/FLTK/blob/master/examples/OpenGL3-glut-test.cxx
  // FL_MULTISAMPLE -> anti-aliasing. WFM.
  Fl::use_high_res_GL(1);
  // TODO: Without this macOS Retina display is a bit wonky TODO: CHECK!!

  size_range( 400, 400 ); 
  // This allows resizing. Without this window is fixed size

  Fl::add_timeout( config::sim_poll_interval, 
                   Display::refresh_simulation, this );
  // Poll simulation for new data periodically
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
  initialize_shaders();
}


const char vertex_shader[] = R"(
#version 150

uniform mat4 camera;
in vec3 vert;

void main() {   
    gl_Position = camera * vec4(vert, 1);
}
)";


const char fragment_shader[] = R"(
#version 150

out vec4 finalColor;

void main() {
    //set every drawn pixel to white
    finalColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";


void
Display::initialize_shaders()
{
  std::vector<sgl::Shader> shaders;
  shaders.push_back( sgl::Shader( vertex_shader, GL_VERTEX_SHADER ) );
  shaders.push_back( sgl::Shader( fragment_shader, GL_FRAGMENT_SHADER ) );
  shader_program.create( shaders );
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
    glViewport( 0, 0, pixel_w(), pixel_h() );
    camera.set_aspect_ratio( (float)pixel_w() / (float)pixel_h() );
  }

  glClearColor(0, 0, 0, 1); // black
  //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glClear( GL_COLOR_BUFFER_BIT );

  render_simulation();

}

void
Display::render_simulation()
{
  glUseProgram( shader_program.id );
  shader_program.set_camera( camera.matrix() );
  for( auto& trajectory : simulation_objects ) trajectory->render();
  glUseProgram( 0 );
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
    double  theta = initial_theta + ( Fl::event_y() - initial_y ) / 5.0,
            phi   = initial_phi   + ( Fl::event_x() - initial_x ) / 5.0;

    camera.orbit_to( phi, theta );
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
      md.drag( Fl::event_x(), Fl::event_y(), camera );
      redraw();      
      return 1;

    case  FL_MOUSEWHEEL:
      {
        if( FL_COMMAND & Fl::event_state() ) {
          camera.dolly_by( Fl::event_dy() );
        } else {
          // scroll in time
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
      // Might need a dispatcher here ...
      switch( Fl::event_text()[0] ) {
        case '[': camera.change_fov( -5 );
                  break;
        case ']': camera.change_fov( +5 );
                  break;
      };
      redraw();
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

void
Display::refresh_simulation( void* ptr )
{
  Display* display = (Display*) ptr;
  
  if( display->simulation == nullptr ) { display->load_simulation(); }
  if( display->simulation->is_stale() ) { display->load_simulation(); }

  float current_jd = display->simulation->get_last_jd();
  if( display->last_display_jd < current_jd ) {
      display->view_simulation();
      display->last_display_jd = current_jd;
      display->redraw();
  }

  Fl::repeat_timeout( config::sim_poll_interval, 
                      Display::refresh_simulation, ptr );
}

void
Display::load_simulation()
{
  DLOG_S(INFO) << "Stale simulation, reloading";

  simulation_objects.clear();
  last_display_jd = 0;

  simulation = simulator.get_simulation();
  for( const auto& ob : simulation->orrery_bodies ) {
    simulation_objects.push_back( 
      std::make_unique<sgl::Trajectory>( ob->name, ob->radius / config::simulation_scale, &shader_program ) );
  }
  for( const auto& ob : simulation->space_ships ) {
    simulation_objects.push_back( 
      std::make_unique<sgl::Trajectory>( ob->name, ob->radius / config::simulation_scale, &shader_program ) );
  }  
}

void
Display::view_simulation()
{
  size_t orrery_body_count = simulation->orrery_bodies.size();
  for( size_t i = 0; i < orrery_body_count; i++ ) {
    simulation_objects[ i ]->copy( simulation->orrery_bodies[ i ], nullptr );
  }
  for( size_t i = 0; i < simulation->space_ships.size(); i++ ) {
    simulation_objects[ orrery_body_count + i ]->copy( simulation->space_ships[ i ], nullptr );
  }  
}


} // namespace sim

