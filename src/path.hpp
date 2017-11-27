/*
  The simulation buffer takes care of position and event history for each
  simulation object.

  There are no synchronization locks here. This is because the simulation will 
  only write to segments that are invisible to the reader, and no writes
  will ever take place to segments visible to the reader.

  It's upto the Simulation to lock access to the entire list/dict of buffers when
  a restart occurs and such locking is not handled at this level. What I envision
  is a read lock at the dictionary level that only locks dictionary deletion
  during simulation restarts.

  See the "Data" writeup in docs for the rationale for this design

  Notes:
    How to instrument a class for the range based for loop apparatus is explained
    in http://antonym.org/2014/02/c-plus-plus-11-range-based-for-loops.html
*/
#pragma once

#include <vector>
#include <array>
#include <cassert>

#include "units.hpp"
#include "event.hpp"
#include "vector.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sim
{

typedef std::vector<Vector> vector_vec_t;


class FractalDownsampler
{
public:
  FractalDownsampler( double rt=1.001, double lt=1e6 ) : 
      ratio_threshold( rt ), linear_threshold( lt )
  {}

  bool operator()( Vector& v )
  {
    cumulative_curve_dist += (v - last_v).norm();
    float linear_dist = (v - last_sample_v).norm();
     
    if(  ( ( cumulative_curve_dist / linear_dist )    > ratio_threshold  )   
       | ( abs( cumulative_curve_dist - linear_dist ) > linear_threshold ) ) 
    {
      cumulative_curve_dist = 0;
      last_sample_v = v;
      last_v = v;
      return true;
    }
    last_v = v;
    return false;
  }

private:
  double     cumulative_curve_dist = 0;
  Vector     last_sample_v = { 0, 0, 0 };
  Vector     last_v = { 0, 0, 0 };
  double     ratio_threshold = 1.001;
  double     linear_threshold = 1e6;
};


struct DataView
// Flattened view (suitable for OpenGL) of data trace in new frame
{
  float*  buf;
  float*  t_buf;
  size_t  n_points;
  size_t  n_points_allocated;
};


class Path
{
public:
  std::string          reference_frame;
  std::string          name;
  vector_vec_t         trajectory;
  std::vector<Event>   events;

public:
  Path( scale=1.0_AU, double rt=1.001, double lt=1e6 ) :
      downsampler( rt, lt ), scale( scale ) 
  void add( Vector& v ) { if( downsampler( v ) ) { trajectory.push_back( v / scale ); } }
  void add( Event& e )  { events.push_back( e ); }
  size_t size() { return trajectory.size(); }
  
  size_t num_points_in_new_frame( SimulationData& ref );
  // The size of the re-referenced data points. The most conservative estmate is
  // the sum of the points in this buffer and the reference buffer.
  
  void copy_to_new_frame( SimulationData& ref, DataView& dv );
  // Given a pre-allocated float buffer fill it with data re-referenced to ref 
  // and re-interpolated.
  // The data is flattened as used by OpenGL. t_buf contains time data

private:
  FractalDownsampler  downsampler;
  double scale = 1.0_AU;

private:
  void flatten( DataView& dv );
  // Used when the referece frame is the one we used originally
  // i.e. no transform has to be done

  Vector interpolate( vector_vec_t& p, size_t n0, float t );
  // Add the interpolated value between n0 and n0 + 1 for time t to the view
  // expects that p[ n0 ].t < t < p[ n0 + 1 ].t
};


} // namespace sim
