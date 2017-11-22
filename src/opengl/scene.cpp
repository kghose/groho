#include "scene.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sgl
{

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
Scene::init()
{
  std::vector<Shader> shaders;
  shaders.push_back( Shader( vertex_shader, GL_VERTEX_SHADER ) );
  shaders.push_back( Shader( fragment_shader, GL_FRAGMENT_SHADER ) );
  shader_program.create( shaders );
}

void 
Scene::render()
{
  glUseProgram( shader_program.id );
  shader_program.set_camera( camera.matrix() );
  for( auto& trajectory : orrery_bodies ) trajectory.render();
  for( auto& trajectory : space_ships ) trajectory.render();
  glUseProgram( 0 );
}

bool
Scene::mirror_simulation( const sim::Simulation& simulation )
{
  bool redraw_needed = false; 

  simulation.lock_before_mirror();

  const sim::orrery_body_vec_t& ob = simulation.get_orrery_bodies();
  const sim::space_ship_vec_t& ss = simulation.get_space_ships();

  if( sim_version_no != simulation.get_sim_version() )
  {
    redraw_needed = true;
    orrery_bodies.clear();
    for( int i = 0; i < ob.size(); i++ )
      orrery_bodies.push_back( Trajectory( &shader_program ) );

    space_ships.clear();
    for( int i = 0; i < ss.size(); i++ )
      space_ships.push_back( Trajectory( &shader_program ) );

    sim_version_no = simulation.get_sim_version();
  }

  for( int i = 0; i < ob.size(); i++ )
    redraw_needed |= orrery_bodies[ i ].copy_simulation_buffer( ob[ i ].simulation_buffer );

  for( int i = 0; i < ss.size(); i++ )
    redraw_needed |= space_ships[ i ].copy_simulation_buffer( ss[ i ].simulation_buffer );

  simulation.unlock_after_mirror();

  return redraw_needed;
}

}