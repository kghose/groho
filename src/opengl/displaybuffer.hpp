/*
  Ok, for the extended discussion see below (YAGNI) but let's do it the
  most aesthetically pleasing way first. This way is memory and CPU inefficient
  but easier to code and has a pleasing abstracation.

  1. There are simulation buffer objects (independent of the Display) that
     stores the position history and events for all simulation objects.
  2. The data is stored in segments.
  3. When the display wants to render, it asks the simulation for this list
     of simulation buffer objects.
  4. The simulation then informs it whether the simulation has been restarted 
     since the last list request, and then gives it a reference/pointer to the
     current list.
  5. If the list hasn't changed the display buffer goes down each each 
     individual simulation buffer and copies over any new segments that have
     been added, creating new VBOs for them as needed.
  6. If the list has changed, all the existing VBOs are destroyed and new ones
     copied over from scratch.

  For synchronization we can get the display to poll the simulation periodically
  for updates, for now.
  
  YAGNI
  =====

  For a while I was trying to completely separate out the simulation code and
  the display (OpenGL) code. Like the simulation objects and simulation
  would have no knowledge of the display at all.

  This is totally possible: after a simulation step we would just go through
  all the simulation objects copying over their state at each time step. 
  This is a little annoying however, since now we have a mirrored list of 
  simulation objects in the display. Whenever the simulation is rerun and
  the objects are changed (e.g. we add or remove an object) we have to mirror
  that operation in the display. Instead of reducing complexity we seem to 
  be increasing it.

  This is a common cost of abstraction, perhaps.

  After some debate I settled on the following. The simulation and simulation
  objects themsleves don't handle any OpenGL stuff, but each simulation object
  does carry this displaybuffer object. They don't know what it does and the
  simulator simply calls the 'add' and 'add_event' functions at each timestep.

  When time comes to display the simulation, the scene gets a copy of all the
  simulation objects and simply calls render on the display buffer. If we 
  restarted the simulation the display buffers are empty, if we changed the 
  number of objects, the list the scene goes through is different, but the
  display code does not need to do anything itself to keep track of this.
  
  The only thing to be careful of is that we a) don't change the list of objects
  when the display is iterating over them, and b) we don't draw from the buffer
  when we are adding simulation points.

  For a) we use a mutex that prevents us from restarting the simulation when the
  display is rendering and vice-versa and b) we have the buffers in multiple
  moderate sized segments and we only write to the last segment, so the display
  can show the previous, finsihed, segments
  
  It does mean that the simulation can not be compiled and used independently
  of the displaybuffer - however when you do a simulation you need some kind of
  output, and I can easily see writing a displaybuffer for SVG output, or output
  to a file and so on and this way we can use the simulation for different
  "front ends".

  The proper way to do this would be to have a pointer to the display buffer
  (which will be more generally called an OutputBuffer) and have all the
  functions be virtual functions. We'll cross that bridge when we come to it.

  But wait! There is a complexity with doing all this in a [different thread][opengl-thread].  
  We need to create multiple contexts and now we are getting pretty complicated.

  [opengl-thread]: https://stackoverflow.com/questions/8912986/opengl-vbo-within-multiple-threads

  I'm going to backpedal a bit about doing things twice if we abstract OpenGL 
  from the simulation code. We abstract enough so that the simulation only needs
  a pointer (to the VBO obtained from glMapBufferRange). When ever the simulation
  restarts the simulation thread stops and then requests the display thread to 
  stop using the current displaybuffers. When the Display acknowledges this
  the simulation 

  1. Have a dictionary/list of all orrery and spaceship objects (we might want to
     treat the two slightly differently, so we keep them separate) for both the
     Display and the Simulation
  2. The dictionary/list in the Simulation carries no history


*/
#pragma once

#include <vector>
#include <array>

#include "opengl.hpp"
#include "timestamps.hpp"
#include "event.hpp"
#include "vector.hpp"


namespace config  // some of these may later be read from config files
{

const size_t scratch_buffer_size = 1000;
const size_t buffer_size = 1000;

}

namespace sgl
{

using namespace sim;

class SimulationBufferSegment
{
public:
  SimulationBufferSegment()
  {
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );

    glBufferData( GL_ARRAY_BUFFER, 
                  sizeof(GLfloat) * 3 * config::buffer_size, NULL, 
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

  bool add( float jd, Vector& v )
  // return true if we need a new buffer segment (and this is done)
  {
    time_stamps[ buffer_index         ] = jd;
    buffer_data[ 3 * buffer_index     ] = v.x;
    buffer_data[ 3 * buffer_index + 1 ] = v.y;
    buffer_data[ 3 * buffer_index + 2 ] = v.z;
    buffer_index++;
    return buffer_index > config::buffer_size;
  }

private:
  ShaderProgram*   shader_program;

  GLuint        vbo;  // might consider using two buffers
  GLuint        vao;
  GLenum  draw_type;
  GLint  draw_start;
  GLint  draw_count;

  GLfloat*                                buffer_data;  //   x,y,z, x,y,z, ...  
  std::array<float, config::buffer_size>  time_stamps;  //   t0,    t1,    t2, ....

  size_t buffer_index;
  // next available location in the index

  std::vector<Event>  events;

};


class SimulationBuffer
{  
public:
  void add( float jd, Vector& v, float tolerance_sq=1.0 )
  {
    if( scratch_buffer_index > 1 )
    {
      if( ( scratch_buffer_index == config::scratch_buffer_size ) |
          ( deviation_from_interpolation_sq( v ) > tolerance_sq ) )
      {
        vertices.push_back( v.x );
        vertices.push_back( v.y );
        vertices.push_back( v.z );
        time_stamps.push_back( jd );
        scratch_buffer_index = 0;
      }
    }
    scratch_buffer[ scratch_buffer_index ] = v;
    scratch_buffer_index++;
  }

  void lock_and_copy()
  {

  }



private:
  float deviation_from_interpolation_sq( Vector& Z )
  {
    Vector X = scratch_buffer[ 0 ],
           Y = scratch_buffer[ scratch_buffer_index / 2 ],
           A = Y - X,
           B = Y - Z,
           L = Z - X;
    
    double a2 = dot( A, A ),
           b2 = dot( B, B ),
           l2 = dot( L, L ),
           l  = std::sqrt( l2 ),
           q  = ( a2 - b2 + l2 ) / ( 2 * l ),
           q2 = q * q;

    return a2 - q2;    
  }

  std::array<Vector, config::scratch_buffer_size> scratch_buffer;
  size_t scratch_buffer_index = 0;


  mutable std::shared_mutex mutex;
};


