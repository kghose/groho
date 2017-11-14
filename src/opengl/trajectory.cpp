#include "shader.hpp"
#include "trajectory.hpp"

namespace sgl
{

void 
Trajectory::update_trajectory()
{
  glDeleteBuffers( 1, &vbo );  
  // "silently ignores 0's and names that do not correspond to existing buffer objects."

  glGenBuffers( 1, &vbo );
  glGenVertexArrays( 1, &vao );

  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  glBufferData( GL_ARRAY_BUFFER, vertices.size(), vertices.data(), GL_DYNAMIC_DRAW );

  glEnableVertexAttribArray( shader_program.get_attrib_loc( "vert" ) );
  glVertexAttribPointer( 
    shader_program.get_attrib_loc( "vert" ), 
    3, 
    GL_FLOAT, 
    GL_FALSE, 
    3 * sizeof( GLfloat ), 
    NULL);

  glBindVertexArray( 0 );  // unbind the VAO
} 

void 
Trajectory::render()
{
  glBindVertexArray( vao );
  glDrawArrays( draw_type, draw_start, draw_count ); 
  // make draw_start, draw_count settable based on where in the sim we want to be
  glBindVertexArray( 0 );
}

} // namespace sgl