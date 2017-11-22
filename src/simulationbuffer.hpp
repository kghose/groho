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

//#include "timestamps.hpp"
#include "event.hpp"
#include "vector.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace config  // some of these may later be read from config files
{

const size_t scratch_buffer_size = 1000;
const size_t buffer_size = 1000;

}


namespace sim
{

class SimulationBufferSegment
{
public:
  bool add( float jd, Vector& v )
  // return true if we need a new buffer segment because this is filled up
  {
    time_stamps[ buffer_index         ] = jd;
    buffer_data[ 3 * buffer_index     ] = v.x;
    buffer_data[ 3 * buffer_index + 1 ] = v.y;
    buffer_data[ 3 * buffer_index + 2 ] = v.z;
    buffer_index++;
    //std::cerr << buffer_index << ", ";
    return buffer_index == config::buffer_size;
  }

  void add( Event& e )
  {
    events.push_back( e );
  }

  const float* buffer_ptr( size_t& buf_size ) const
  {
    buf_size = buffer_index;
    return buffer_data.data();
  }

  const std::array<float, config::buffer_size> copy_of_time_stamps() const { return time_stamps; }
  const std::vector<Event> copy_of_events() const { return events; }


private:
  std::array<float, 3 * config::buffer_size>  buffer_data;  //   x,y,z, x,y,z, ...  
  std::array<float, config::buffer_size>      time_stamps;      //   t0,    t1,    t2, ....
  size_t buffer_index = 0;
  // next available location in the index for insertion
  std::vector<Event>  events;
};


struct SBSNode;
typedef std::shared_ptr<SBSNode> SBSNode_ptr_t;


struct SBSNode
{
  SimulationBufferSegment buffer;
  SBSNode_ptr_t next;

  SBSNode_ptr_t append_new_segment()
  {
    SBSNode_ptr_t new_segment = SBSNode_ptr_t( new SBSNode );
    next = new_segment;
    return new_segment;
  }
};


class SimulationBuffer
{
public:
  float tolerance_sq = 0;

public:
  SimulationBuffer()
  {
    head_segment = SBSNode_ptr_t( new SBSNode);
    last_segment = head_segment;
    node_count = 1;
    available_node_count = 0;
  }

  void add( float jd, Vector& v )
  {
    assert( node_count != available_node_count );  // we should not have finalized it
    if( scratch_buffer_index > 1 )
    {
      if( ( scratch_buffer_index == config::scratch_buffer_size ) |
          ( deviation_from_interpolation_sq( v ) > tolerance_sq ) )
      {
        if( last_segment->buffer.add( jd, v ) ) {  
          last_segment = last_segment->append_new_segment();
          node_count++; 
          available_node_count++;
        }
        scratch_buffer_index = 0;
      }
    }
    scratch_buffer[ scratch_buffer_index ] = v;
    scratch_buffer_index++;
  }

  size_t available_size() const { return available_node_count; }
  void finalize() { available_node_count = node_count; }

  const SimulationBufferSegment& operator[] ( size_t index ) const
  {
    if( index < available_node_count )
    {
      SBSNode_ptr_t  this_node = head_segment;
      while( index > 0 ) {
        this_node = this_node->next;
        index--;
      }
      assert( this_node != nullptr );
      return this_node->buffer;
    }
    else
    {
      std::runtime_error( "Index : "+ std::to_string(index) + "is out of range!" );
    }
  }

private:
  float deviation_from_interpolation_sq( Vector& Z )
  {
    Vector X = scratch_buffer[ 0 ],
           Y = scratch_buffer[ scratch_buffer_index / 2 ],
           A = Y - X,
           B = Y - Z,
           L = Z - X;
    
    double a2 = dot( A, A ),
           b2 = dot( B, B ),
           l2 = dot( L, L ),
           l  = std::sqrt( l2 ),
           q  = ( a2 - b2 + l2 ) / ( 2 * l ),
           q2 = q * q;

    return a2 - q2;    
  }

private:
  std::array<Vector, config::scratch_buffer_size> scratch_buffer;
  size_t scratch_buffer_index = 0;

  SBSNode_ptr_t   head_segment;
  SBSNode_ptr_t   last_segment;

  size_t     node_count;
  size_t     available_node_count;  
             // The ones that are safe to copy. usually node_count - 1 
             // unless finalized

};


} // namespace sim