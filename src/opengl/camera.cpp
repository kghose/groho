#include "camera.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sgl
{

Camera::Camera() :
    target( 0, 0, 0 ),
    dist( 120.0 ),
    phi( 0 ),
    theta( 45 ),
    fov( 40 ),
    near_plane( 5 ),
    far_plane( 500.0 ),
    aspect_ratio( 3.0 / 4.0 )
{
}

glm::mat4
Camera::matrix() const
// https://stackoverflow.com/a/11404171/2512851
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
{
  glm::mat4 projection = 
    glm::perspective( glm::radians( fov ), aspect_ratio, near_plane, far_plane );
  
  //glm::mat4 view = glm::translate( rotation_mat(), -pos );
  glm::mat4 view = glm::lookAt( 
    glm::vec3( dist, 0, 0),
    glm::vec3( 0, 0, 0),
    glm::vec3( 0, 0, 1) 
  );

  glm::mat4 model(1.0f);
  model = glm::translate( model, -target );
  model = glm::rotate( model, glm::radians( theta ), glm::vec3(  0.0f, 1.0f, 0.0f ) );
  model = glm::rotate( model, glm::radians(  phi  ), glm::vec3(  0.0f, 0.0f, 1.0f ) );
  return projection * view * model;
}

void 
Camera::set_target( glm::vec3 _target ) 
{ 
  target = _target; 
}

void 
Camera::dolly_by( float x )
{
  dist += x;
  if( dist < 0 ) dist = 0;
}

void 
Camera::orbit_to( float new_phi, float new_theta )
{
  phi = std::fmodf( new_phi, 360.0f );
  if( phi < 0) phi += 360.0f;
  
  theta = new_theta;
  if( theta > 90) theta = 90;
  if( theta < -90) theta = -90;
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

} // namespace sgl