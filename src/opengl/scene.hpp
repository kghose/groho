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


namespace sgl
{

class Scene
{
public:
  Camera camera;

  void init();
  void render();

  void mirror_simulation( const sim::Simulation& simulation );

private:
  ShaderProgram shader_program;

  std::unordered_map<std::string, Trajectory>    simulation_objects;
  int  sim_version_no = -1;

  std::unordered_map<std::string, Trajectory>    ships;
  std::unordered_map<std::string, Trajectory>  planets;
};

} // namespace opengl

