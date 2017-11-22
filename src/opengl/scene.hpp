/*
  Carries cameras and objects in a scene. Hides most of the nastiness of the
  OpenGL functions behind a blander interface.
*/
#pragma once

#include <vector>

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

  std::vector<Trajectory>    orrery_bodies;
  std::vector<Trajectory>    space_ships;

  int  sim_version_no = -1;
};

} // namespace sgl

