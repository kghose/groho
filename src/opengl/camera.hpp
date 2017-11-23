/*
  Handles camera transformations 
*/

#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace sgl
{


class Camera
{
public:
  Camera();

  glm::mat4 matrix() const;

  void set_target( glm::vec3 _pos );  // new camera target
  void dolly_by( float x );           // move towards/away from target
  
  double get_phi() { return phi; }     // horizontal
  double get_theta() { return theta; } // vertical
  void orbit_to( float new_phi, float new_theta );

  void set_fov( float _fov );
  void change_fov( float dfov );

  void set_planes( float near, float far );
  void set_aspect_ratio( float ar );

private:
  glm::vec3    target;
  float        dist;
  float        phi;
  float        theta;
  float        fov;
  float        near_plane;
  float        far_plane;
  float        aspect_ratio;
};


} // namespace sgl

