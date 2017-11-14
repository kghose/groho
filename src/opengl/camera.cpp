#include "camera.hpp"

namespace sgl
{

Camera::Camera() :
    pos( 0, 0, 1.0 ),
    phi( 0 ),
    theta( 0 ),
    fov( 55 ),
    near_plane( 0.1 ),
    far_plane( 100.0 ),
    aspect_ratio( 3.0 / 4.0 )
{
}

glm::mat4
Camera::rotation_mat() const
{
  glm::mat4 dir;
  dir = glm::rotate( dir, glm::radians( theta ), glm::vec3( 1, 0, 0 ) );
  dir = glm::rotate( dir, glm::radians(  phi  ), glm::vec3( 0, 1, 0 ) );
  return dir;
}

glm::mat4 
Camera::projection_mat() const 
{
  return glm::perspective( 
      glm::radians( fov ), aspect_ratio, near_plane, far_plane );
}

glm::mat4 
Camera::view_mat() const 
{
  return rotation_mat() * glm::translate( glm::mat4(),  -pos );
}

glm::mat4 
Camera::full_mat() const 
{
    return projection_mat() * view_mat();
}

void 
Camera::set_position( glm::vec3 _pos ) 
{ 
  pos = _pos; 
}

glm::vec3 
Camera::orientation() const
{
  return glm::vec3(
    glm::inverse( rotation_mat() ) * glm::vec4( 0, 0, -1, 1 )
  );
}

void 
Camera::dolly_by( float x )
{
  pos += x * orientation();
}

void 
Camera::pan_to( float new_phi, float new_theta )
{
  phi = new_phi;
  theta = new_theta;
  wrap_angle( phi );
  wrap_angle( theta );
}

void 
Camera::set_fov( float _fov ) 
{ 
  fov = _fov; 
}

void 
Camera::change_fov( float dfov ) 
{ 
  fov += dfov; 
}

void 
Camera::set_planes( float near, float far ) 
{ 
  near_plane = near; 
  far_plane = far; 
}

void 
Camera::set_aspect_ratio( float ar ) 
{ 
  aspect_ratio = ar; 
}

void 
Camera::wrap_angle( float x )
{
  x = std::fmodf( x, 360.0f );
  x = x < 0 ? -x : x;  
}

} // namespace sgl