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
  glm::vec3 direction() const;

  void set_position( glm::vec3 _pos ); // camera sits here
  void dolly_by( float x );            // move in/out along the camera direction
  
  double get_phi() { return phi; }     // horizontal
  double get_theta() { return theta; } // vertical
  void pan_to( float new_phi, float new_theta );

  void set_fov( float _fov );
  void change_fov( float dfov );

  void set_planes( float near, float far );
  void set_aspect_ratio( float ar );

private:
  glm::vec3    pos = {0, 0, 40};

  float        phi = 0,
             theta = 0,
               fov = 55,
        near_plane = 0.1,
         far_plane = 100,
      aspect_ratio = 1.0;

  glm::mat4 rotation_mat() const;      // 
  void wrap_angle( float x );
};


} // namespace sgl

