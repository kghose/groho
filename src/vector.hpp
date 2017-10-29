#pragma once 

#include <cmath>
#include <iostream>

namespace sim 
{

struct Vector 
{
  double x, y, z;

  double norm_sq() { return x*x + y*y + z*z; }
  double norm()    { return std::sqrt( norm_sq() ); }

  Vector
  operator+( const Vector& rhs )
  {
    return Vector {
      .x = x + rhs.x,
      .y = y + rhs.y,
      .z = z + rhs.z
    };
  }

  Vector
  operator-( const Vector& rhs )
  {
    return Vector {
      .x = x - rhs.x,
      .y = y - rhs.y,
      .z = z - rhs.z
    };
  }

  Vector
  operator/( const double rhs )
  {
    return Vector {
      .x = x / rhs,
      .y = y / rhs,
      .z = z / rhs
    };
  }
  
  Vector& 
  operator+=( const Vector& rhs )
  {
    x += rhs.x; y += rhs.y; z += rhs.z;
    return *this;
  }

  Vector&
  operator=( const Vector& rhs )
  {
    x = rhs.x; y = rhs.y; z = rhs.z;
    return *this;
  }
};

inline Vector 
operator*( double lhs, const Vector& rhs )
{
  return Vector { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
}

inline Vector 
operator*( const Vector& lhs, double rhs )
{
  return Vector { rhs * lhs.x, rhs * lhs.y, rhs * lhs.z };
}

inline std::ostream& 
operator<<( std::ostream &os, const Vector&  v ) 
{
  os << v.x << ", " << v.y << ", " << v.z;
  return os;
}


struct LatLon 
{
  float lat, lon;
  //LatLon( float _lat, float _lon ) { lat = _lat; lon = _lon; }
};

inline std::ostream&
operator<<( std::ostream &os, const LatLon& ll )
{
  os << ll.lat << ", " << ll.lon;
  return os;
}

} // namespace sim