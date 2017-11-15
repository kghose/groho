#include "shader.hpp"
#include "trajectory.hpp"

namespace sgl
{


Trajectory::Trajectory( ShaderProgram* shdr ) :
    shader_program( shdr ), 
    vbo( 0 ),
    vao( 0 ),
    draw_type( GL_LINE_STRIP ),
    //draw_type( GL_TRIANGLES ),
    draw_start( 0 ),
    draw_count( 0 )
{}

void 
Trajectory::update_trajectory()
{
  glDeleteBuffers( 1, &vbo );  
  // "silently ignores 0's and names that do not correspond to existing buffer objects."

  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  glGenBuffers( 1, &vbo );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  glBufferData( GL_ARRAY_BUFFER, 
                sizeof(GLfloat) * vertices.size(), vertices.data(), 
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

void 
Trajectory::render()
{
  glBindVertexArray( vao );
  glDrawArrays( draw_type, draw_start, draw_count ); 
  // make draw_start, draw_count settable based on where in the sim we want to be
  glBindVertexArray( 0 );
}

} // namespace sgl