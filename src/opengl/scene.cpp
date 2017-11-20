#include "scene.hpp"

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
  for( auto& [ name, trajectory ] : planets ) trajectory.render();
  for( auto& [ name, trajectory ] : ships ) trajectory.render();
  glUseProgram( 0 );
}

void 
Scene::mirror_simulation( const sim::Simulation& simulation )
{
  simulation.lock_before_mirror();
  if( sim_version_no != simulation.get_sim_version() )
  {
    simulation_objects.clear();
    for( const auto& [ name, body ] : simulation.get_simulation_objects() )
    {
      simulation_objects.insert( { name, Trajectory( &shader_program ) } );
    }
    sim_version_no = simulation.get_sim_version();
  }

  for( const auto& [ name, body ] : simulation.get_simulation_objects() )
  {
    simulation_objects[ name ].copy_simulation_buffer( body->simulation_buffer );
  }
  simulation.unlock_after_mirror();
}

}