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

  glm::mat4 projection_mat () const;
  glm::mat4 view_mat() const;
  glm::mat4 full_mat() const;

  void set_position( glm::vec3 _pos ); // camera sits here
  glm::vec3 orientation() const;       // camera points this way
  void dolly_by( float x );            // move in/out along the camera direction
  
  double get_phi() { return phi; }     // horizontal
  double get_theta() { return theta; } // vertical
  
  void pan_to( float new_phi, float new_theta );

  void set_fov( float _fov );
  void change_fov( float dfov );

  void set_planes( float near, float far );
  void set_aspect_ratio( float ar );

private:
  glm::vec3    pos;

  float        phi,
             theta,
               fov,
        near_plane,
         far_plane,
      aspect_ratio;

  glm::mat4 rotation_mat() const;      // 

  void wrap_angle( float x );
};


} // namespace sgl

