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
Scene::add_planet( std::string name )
{
  //planets.emplace( name, std::ref(shader_program) );
  planets[ name ] = Trajectory( &shader_program );
}

}