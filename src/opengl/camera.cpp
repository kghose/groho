#include "camera.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sgl
{

Camera::Camera() :
    pos( 0, 0, 120.0 ),
    phi( 0 ),
    theta( 0 ),
    fov( 40 ),
    near_plane( 10 ),
    far_plane( 1000.0 ),
    aspect_ratio( 3.0 / 4.0 )
{
}

glm::mat4
Camera::matrix() const
// https://stackoverflow.com/a/11404171/2512851
{
  glm::mat4 projection = 
    glm::perspective( glm::radians( fov ), aspect_ratio, near_plane, far_plane );
  
  glm::mat4 view = glm::translate( rotation_mat(), -pos );
  
  return projection * view;
}

glm::vec3
Camera::direction() const
{
  return glm::vec3( glm::inverse( rotation_mat() ) * glm::vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );
}

glm::mat4
Camera::rotation_mat() const
{
  glm::mat4 rot(1.0f);
  rot = glm::rotate( rot, glm::radians( theta ), glm::vec3( -1.0f, 0.0f, 0.0f ) );
  rot = glm::rotate( rot, glm::radians(  phi  ), glm::vec3(  0.0f, 1.0f, 0.0f ) );
  return rot;
}

void 
Camera::set_position( glm::vec3 _pos ) 
{ 
  pos = _pos; 
}

void 
Camera::dolly_by( float x )
{
  pos += x * direction();
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
  fov = fov <= 1 ? 1 : fov;
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