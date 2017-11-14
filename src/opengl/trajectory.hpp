/*
  Represents trajectory data for a spacecraft. For spacecraft the data array
  keeps track of all data points for the whole simulation while for Orrery
  objects one orbit is kept. 

  Since the shader program and shaders are identical for all trajectories they
  are not included in the trajectory

  Memory computations: 
    4 * 3 = 12 bytes per point
    One million points ~ 12 MB

*/
#pragma once

#include <vector>

#include "opengl.hpp"
#include "shader.hpp"
#include "vector.hpp"


namespace sgl
{

struct Trajectory
{
  ShaderProgram&   shader_program;

  GLuint        vbo;  // might consider using two buffers
  GLuint        vao;
  GLenum  draw_type;
  GLint  draw_start;
  GLint  draw_count;

  std::vector<GLfloat> vertices;

  Trajectory( ShaderProgram& shader_program ) :
      shader_program( shader_program ), 
      vbo( 0 ),
      vao( 0 ),
      draw_type( GL_LINE_SMOOTH ),
      draw_start( 0 ),
      draw_count( 0 )
  {}
  
  void add_point( sim::Vector& v )  
  // Add this point to our internal buffer
  {
    vertices.push_back( v.x );  // what happens with the float double conversion?
    vertices.push_back( v.y );
    vertices.push_back( v.z );
  }

  void update_trajectory();  
  // when we are ready, we make the latest data the display data

  void render();
  // wraps the GL calles needed to render this trajectory
};

}


