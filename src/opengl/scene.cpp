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
Scene::initialize_shaders()
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
  for( auto& [ name, trajectory ] : orrery_bodies ) trajectory.render();
  for( auto& [ name, trajectory ] : space_ships ) trajectory.render();
  glUseProgram( 0 );
}

bool
Scene::needs_redraw()
{
  return true;  // make smarter
}

void 
Scene::initialize( 
    const sim::orrery_body_vec_t& _orrery_bodies, 
    const sim::space_ship_vec_t& _space_ships )
// If there is a chance the list of simulation objects could have changed,
// call this to recreate the views
{
  orrery_bodies.clear();
  space_ships.clear();

  for( const auto& b : _orrery_bodies ) {
    orrery_bodies.insert( { b.name, Trajectory( b.name, &shader_program ) } );
  }
  for( const auto& b : _space_ships ) {
    space_ships.insert( { b.name, Trajectory( b.name, &shader_program ) } );
  }

  // We generally want to keep the existing target but in rare cases, if we 
  // switch orrery models, the name may not exist and we need to find a default one
  // This is also triggered on first run
  if( ( orrery_bodies.count( target_frame ) == 0 ) & ( _orrery_bodies.size() ) ) {
    target_frame = _orrery_bodies[ 0 ].name;
  }
}

sim::PathView& 
Scene::get_path_view_orrery_body( std::string name )
{
  return orrery_bodies[ name ];
}

sim::PathView& 
Scene::get_path_view_space_ship( std::string name )
{
  return space_ships[ name ];
}


} // namespace sgl