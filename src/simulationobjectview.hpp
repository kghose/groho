/*
  Given two simulationobjects A and B, transform A such that it is in the
  reference frame formed by B. Write the data into a flat buffer that could
  be from OpenGL  
*/
#pragma once

#include <vector>
#include <array>
#include <cassert>

#include "simulationobject.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sim
{

class SimulationObjectView
{
public:
  SimulationObjectView( cnst_so_shptr obj, cnst_so_shptr ref ) :
    obj( obj ), ref( ref )
  {}

  size_t max_size();
  // when we transform obj relative to ref, because of adpative downsampling we
  // don't know ahead of time the exact number of points the downsampled data will
  // take, but we do know the maximum possible.

  size_t path_view( float* buf, float* t_buf, size_t idx );
  // Transform obj relative to ref and write out into buf
  // take the first idx points
  // write timestamps into t_buf
  // return the actual size written

private:
  cnst_so_shptr obj; 
  cnst_so_shptr ref;

  size_t copy( float* buf, float* t_buf, size_t idx);
  // Write the path of a into a flat buffer that could be from OpenGL

  size_t transform( float* buf, float* t_buf, size_t idx );
  // Given two simulationobjects A and B, transform A such that it is in the
  // reference frame formed by B. Write the data into a flat buffer that could
  // be from OpenGL
};




/*
class Path
{
public:
  vector_vec_t         trajectory;
  std::vector<Event>   events;
  std::string          name;
  std::string          reference;

public:
  Path( std::string name, std::string reference, 
        double scale=1.0_AU, double rt=1.001, double lt=1e6 ) : 
      name( name ), reference( reference ),
      sample_this_point( rt, lt ), scale( scale )
  {}
  void add( Vector& v ) { if( sample_this_point( v ) ) { trajectory.push_back( v / scale ); } }
  void add( Event& e )  { events.push_back( e ); }
  size_t size() { return trajectory.size(); }
  
  size_t num_points_in_new_frame( Path& new_frame );
  // The size of the re-referenced data points. The most conservative estmate is
  // the sum of the points in this buffer and the reference buffer.

  void view_in_frame( Path& new_frame, PathView& dv );
  // Given a pre-allocated float buffer fill it with data re-referenced to ref 
  // and re-interpolated.
  // The data is flattened as used by OpenGL. t_buf contains time data

private:
  FractalDownsampler  sample_this_point;
  double scale = 1.0_AU;

private:
  void view_in_new_frame( Path& new_frame, PathView& dv );
  // Used when the referece frame is different form the one we used originally
  // i.e. transform s required

  void view_in_existing_frame( PathView& dv );
  // Used when the referece frame is the one we used originally
  // i.e. no transform has to be done

  Vector interpolate( vector_vec_t& p, size_t n0, float t );
  // Add the interpolated value between n0 and n0 + 1 for time t to the view
  // expects that p[ n0 ].t < t < p[ n0 + 1 ].t
};
*/

} // namespace sim
