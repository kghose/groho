#include "shader.hpp"
#include "trajectory.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sgl
{


TrajectorySegment::TrajectorySegment( 
    ShaderProgram* shdr, 
    const sim::SimulationBufferSegment& sim_buf ) :
      shader_program( shdr ), 
      draw_type( GL_LINE_STRIP ),
      //draw_type( GL_TRIANGLES ),
      draw_start( 0 )
{
  GLfloat *vertex_buf = (GLfloat*) sim_buf.buffer_ptr( num_points );
  time_stamps = sim_buf.copy_of_time_stamps();
  events = sim_buf.copy_of_events();

  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  glBufferData( GL_ARRAY_BUFFER, 
                sizeof(GLfloat) * 3 * num_points , 
                vertex_buf, 
                GL_DYNAMIC_DRAW );

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

TrajectorySegment::~TrajectorySegment()
{
  glDeleteBuffers( 1, &vbo );  
  // "silently ignores 0's and names that do not correspond to existing buffer objects."  
}

void 
TrajectorySegment::render()
{
  draw_start = 0;
  draw_count = 3 * num_points;
  // make draw_start, draw_count settable based on where in the sim we want to be

  glBindVertexArray( vao );
  glDrawArrays( draw_type, draw_start, draw_count ); 
  glBindVertexArray( 0 );
}


bool
Trajectory::copy_simulation_buffer( const sim::SimulationBuffer& sb )
{
  bool copy_happened = false;
  
  int already_copied_count = segments.size();
  int available_count = sb.available_size();
  for( int i = already_copied_count; i < available_count; i++ )
  {
    segments.push_back( TrajectorySegment( shader_program, sb[ i ] ) );
    copy_happened = true;
  }
  
  return copy_happened;
}

void 
Trajectory::render()
{
  for( auto& segment : segments ) segment.render();
}


} // namespace sgl