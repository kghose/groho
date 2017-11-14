#include "scene.hpp"

namespace sgl
{

void
Scene::init()
{
  std::vector<Shader> shaders;
  shaders.push_back( Shader( vertex_shader, GL_VERTEX_SHADER ) );
  // shaders.push_back( Shader( fragment_shader, GL_FRAGMENT_SHADER ) );
  shader_program.create( shaders );
}

void 
Scene::render()
{
  glUseProgram( shader_program.id );
  shader_program.set_camera( camera.full_mat() );
  for( auto& [ name, trajectory ] : planets ) trajectory.render();
  glUseProgram( 0 );
}

}