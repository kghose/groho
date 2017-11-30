#include "shader.hpp"
#include "trajectory.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sgl
{

const size_t initial_buffer_size = 1000;

const GLenum  draw_type = GL_LINE_STRIP;
const GLenum  buffer_type = GL_STATIC_DRAW;


Trajectory::Trajectory( std::string name, ShaderProgram* shdr ) : 
    PathView( name ), shader_program( shdr )
{
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  // allocate( initial_buffer_size ); // do we need to do this?

  glEnableVertexAttribArray( shader_program->get_attrib_loc( "vert" ) );
  glVertexAttribPointer( 
      shader_program->get_attrib_loc( "vert" ), 
      3, 
      GL_FLOAT, 
      GL_FALSE, 
      0, 
      NULL
  );

  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
  glBindVertexArray( 0 );   
}

Trajectory::~Trajectory()
{
  glDeleteBuffers( 1, &vbo );
  glDeleteVertexArrays( 1, &vao );
  // "silently ignores 0's and names that do not correspond to existing buffer objects."  
}

void
Trajectory::allocate( size_t expected_points )
{  
  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  if( expected_points > buf_allocated_points )
  {
    glBufferData( 
      GL_ARRAY_BUFFER, 
      sizeof(GLfloat) * 3 * expected_points, 
      nullptr, 
      buffer_type 
    );

    t_buf = std::vector<float>( expected_points ) ;
    buf_allocated_points = expected_points;
  }

  buf = (float*) glMapBufferRange( GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * 3 * expected_points, GL_MAP_WRITE_BIT );

  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
}

void 
Trajectory::finalize()
{
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glUnmapBuffer( GL_ARRAY_BUFFER );
  glBindBuffer(GL_ARRAY_BUFFER, 0);
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