#pragma once
# include "vector.hpp"

namespace sim
{

class BallOfMud
{
public:  
  std::string   name;
  Vector         pos;

  double          GM, // needed for gravity calculations 
              radius,      // m needed surface calculations (launch/crash)
             ra, dec,     // radians
            rotation;    // radians / day
            // https://en.wikipedia.org/wiki/Axial_tilt#Solar_System_bodies
  BallOfMud(
      std::string  name,
      Vector        pos,
      double         GM,
      double     radius,
      double         ra,
      double        dec,
      double   rotation )
      :
      name( name ),
      pos( pos ),
      GM( GM ),
      radius( radius ),
      ra( ra ),
      dec( dec ),
      rotation( rotation )
  {}
      
  virtual void 
  propagate( double jd ) = 0;

  // Given a surface position in (lat, lon) return the absolute
  // position in the simulation space
  Vector 
  position_in_space( const LatLon &l ) 
  {
    // TODO: Need to figure out meanings of ra, dec
    return Vector();
  }

  // Given a position in space return the projection of that point
  // onto the surface of this ball of mud
  LatLon 
  projected_lat_long( const Vector &v ) 
  {
    // TODO: Need to figure out meanings of ra, dec
    return LatLon();
  }
};
  
}