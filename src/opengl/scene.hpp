/*
  Carries cameras and objects in a scene. Hides most of the nastiness of the
  OpenGL functions behind a blander interface.
*/
#pragma once

#include <unordered_map>

#include "opengl.hpp"
#include "camera.hpp"
#include "trajectory.hpp"


namespace sgl
{

class Scene
{
public:
  Camera camera;

  std::unordered_map<std::string, Trajectory>    ships;
  std::unordered_map<std::string, Trajectory>  planets;

  void init();
  void render();

private:
  ShaderProgram shader_program;
};

} // namespace opengl

