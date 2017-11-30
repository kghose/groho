/*
  This is the interface between the Path object and any system that wants
  to copy over the data for display purposes
*/
#pragma once

#include <vector>
#include <stdexcept>


namespace sim
{

struct PathView
// Flattened view (suitable e.g. for OpenGL) of data trace in new frame
{
  float*   buf    = nullptr;         // typically will use OpenGL to allocate this pointer
  std::vector<float> t_buf;
  size_t   buf_allocated_points = 0;  // actual size in bytes is sizeof(datatype) * 3 * buf_allocated_points
  size_t   num_points = 0;

  const std::string          name;
  std::string                reference;

  PathView( std::string name ) : name( name ) {};

  virtual void allocate( size_t expected_points ) = 0;
  // allocate (or reallocate) storage for the data.
  // pass this pointer to buf


  // {
  //   buf = really_allocate( n_pts );
  //   if( buf == nullptr ) std::runtime_error( "Allocation for " + name + " failed" );
  // }

  virtual void finalize() = 0;
  // any steps needed to indicate the data have been written and are ready
};

} // namespace sim