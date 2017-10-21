#pragma once 

#include <cmath>
#include <iostream>

namespace sim 
{

struct Vector 
{
  double x, y, z;
  inline double norm_sq() { return x * x + y * y + z * z; }
  inline double norm()    { return std::sqrt(norm_sq()); }
};

inline std::ostream& operator<<( std::ostream &os, const Vector&  v) 
{
  os << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
  return os;
}


struct LatLon 
{
  double lat, lon;
};

inline std::ostream &operator<<( std::ostream &os, const LatLon& ll )
{
  os << "(" << ll.lat << ", " << ll.lon << ")" << std::endl;
  return os;
}

} // namespace sim