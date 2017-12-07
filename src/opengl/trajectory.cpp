#include "shader.hpp"
#include "trajectory.hpp"
#include "simulationobjectview.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sgl
{

const size_t initial_buffer_size = 1000;

const GLenum  draw_type = GL_LINE_STRIP;
const GLenum  buffer_type = GL_STATIC_DRAW;


Trajectory::~Trajectory()
{
  clear();
}


void
Trajectory::clear()
{
  glDeleteBuffers( 1, &vbo );
  glDeleteVertexArrays( 1, &vao );
  // "silently ignores 0's and names that do not correspond to existing buffer objects."  
}

void
Trajectory::copy( sim::cnst_so_shptr obj, sim::cnst_so_shptr ref )
{
  clear();
  sim::SimulationObjectView sov( obj, ref );

  size_t max_points = sov.max_size();

  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  glBufferData( GL_ARRAY_BUFFER, 
                sizeof(GLfloat) * 3 * max_points, 
                nullptr, 
                GL_DYNAMIC_DRAW );

  GLfloat* buf = (GLfloat *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  
  t_buf = std::unique_ptr<float>( new float[ max_points ] );
  num_points = sov.path_view( buf, t_buf.get(), max_points );

  glUnmapBuffer(GL_ARRAY_BUFFER);
    
  glEnableVertexAttribArray( shader_program->get_attrib_loc( "vert" ) );
  glVertexAttribPointer( 
    shader_program->get_attrib_loc( "vert" ), 
    3, 
    GL_FLOAT, 
    GL_FALSE, 
    0, //3 * sizeof( GLfloat ), 
    NULL);

  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
  glBindVertexArray( 0 );  
}

void 
Trajectory::render()
{
  draw_start = 0;
  draw_count = num_points;
  // make draw_start, draw_count settable based on where in the sim we want to be

  glBindVertexArray( vao );
  glDrawArrays( draw_type, draw_start, draw_count ); 
  glBindVertexArray( 0 );
}

} // namespace sgl