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

  //Todo: better information hiding on part of simulation
  void mirror_orrery( sim::Simulation& simulation );
  //Trajectory& planet( std::string name ) { return planets[ name ]; }

private:
  ShaderProgram shader_program;
  std::unordered_map<std::string, Trajectory>    ships;
  std::unordered_map<std::string, Trajectory>  planets;
};

} // namespace opengl

