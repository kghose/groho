/*
  Carries cameras and objects in a scene. Hides most of the nastiness of the
  OpenGL functions behind a blander interface.
*/
#pragma once

#include <unordered_map>

#include "opengl.hpp"
#include "camera.hpp"
#include "trajectory.hpp"
#include "simulation.hpp"
#include "simulationview.hpp"


namespace sgl
{

class Scene : public sim::SimulationView
{
public:
  Camera camera;

  void initialize_shaders();
  void render();

  bool needs_redraw();
  // returns true if data has changed

  void initialize( 
      const sim::orrery_body_vec_t& _orrery_bodies, 
      const sim::space_ship_vec_t& _space_ships );

  sim::PathView& get_path_view_orrery_body( std::string name );
  sim::PathView& get_path_view_space_ship( std::string name );

private:
  ShaderProgram shader_program;

  std::unordered_map<std::string, Trajectory>  orrery_bodies;
  std::unordered_map<std::string, Trajectory>  space_ships;

};

} // namespace sgl

