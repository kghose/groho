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

#include <memory>
#include <vector>
#include <array>
#include <cassert>

//#include "timestamps.hpp"
#include "event.hpp"
#include "vector.hpp"


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
  size_t buffer_index;
  // next available location in the index for insertion
  std::vector<Event>  events;
};


struct SBSNode;
typedef std::shared_ptr<SBSNode> SBSNode_ptr_t;


struct SBSNode
{
  SimulationBufferSegment buffer;
  bool ready = false;  // this segment is ready to be read from
  SBSNode_ptr_t next;

  SBSNode_ptr_t append_new_segment()
  {
    ready = true;
    SBSNode_ptr_t new_segment = SBSNode_ptr_t( new SBSNode );
    next = new_segment;
    return new_segment;
  }
};

class SBSNodeIter
{
public:
  SBSNodeIter( SBSNode_ptr_t const node ) : node( node ) {}

  bool operator!=( const SBSNodeIter& other )
  // We have our own way of flagging if the iteration should stop here
  // basically we stop at a node that is not ready
  {
    if( node == nullptr ) return false;
    return node->ready;
  }

  const SBSNodeIter& operator++()
  { 
    node = node->next;    
    return *this;
    // the_end is checked differently
  }

  const SimulationBufferSegment& operator*() const { return node->buffer; }
  // Note that we don't give access to the linked list, just the buffer

private:
  SBSNode_ptr_t  node;
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
  }

  // ~SimulationBuffer()
  // {
  //   while( head_segment != nullptr )
  //   {
  //     SBSNode* next = head_segment->next;
  //     delete head_segment;
  //     head_segment = next;
  //   }
  // }
  
  void add( float jd, Vector& v )
  {
    assert( !last_segment->ready );
    if( scratch_buffer_index > 1 )
    {
      if( ( scratch_buffer_index == config::scratch_buffer_size ) |
          ( deviation_from_interpolation_sq( v ) > tolerance_sq ) )
      {
        if( last_segment->buffer.add( jd, v ) ) {  
          last_segment = last_segment->append_new_segment();
          node_count++; 
        }
        scratch_buffer_index = 0;
      }
    }
    scratch_buffer[ scratch_buffer_index ] = v;
    scratch_buffer_index++;
    // std::cerr << scratch_buffer_index << ", ";
  }

  size_t size() { return node_count; }
  void finalize() { last_segment->ready = true; }

  SBSNodeIter begin() const
  {
    return SBSNodeIter( head_segment );
  }  

  SBSNodeIter end() const
  {
    return SBSNodeIter( last_segment ); // not really used
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
};


} // namespace sim