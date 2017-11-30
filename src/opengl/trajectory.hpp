/*
  This copies trajectory data 

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

#include "opengl.hpp"
#include "shader.hpp"
#include "pathview.hpp"

namespace sgl
{


// class TrajectorySegment
// {
// public:
//   TrajectorySegment( ShaderProgram* shdr, const sim::SimulationBufferSegment& sim_buf );
//   ~TrajectorySegment();
//   void render();
//   // wraps the GL calls needed to render this trajectory

// private:
//   ShaderProgram*   shader_program;

//   GLuint        vbo;
//   GLuint        vao;
//   GLenum  draw_type;
//   GLint  draw_start;
//   GLint  draw_count;
//   size_t num_points;

//   std::array<float, config::buffer_size>  time_stamps;
//   // next available location in the index for insertion
//   std::vector<sim::Event>  events;  
// };

class Trajectory : public sim::PathView
{
public:
  Trajectory() : PathView( "nothing" ) {} // needed for unordered_map
  Trajectory( std::string name, ShaderProgram* shdr);
  ~Trajectory();

  void render();
  // wraps the GL calls needed to render this trajectory

  void allocate( size_t expected_points );
  void finalize();

private:
  ShaderProgram*   shader_program;

  GLuint        vbo;
  GLuint        vao;
  GLint  draw_start = 0;
  GLint  draw_count;  
};

}


