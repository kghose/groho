/*
  We need 
    - To store simulation data so that OpenGL can efficiently get at them.
    - To break up the data so that as the simulation progressed or when 
      we recomputed the simulation we didn't have to recopy all the data.
    - To synchronize a suitable simulation checkpoint so that we can rerun
      simulations at the granularity of a segment size.
*/
#pragma once

#include <vector>
#include <unordered_map>

#include "timestamps.hpp"
#include "event.hpp"
#include "vector.hpp"


namespace sim
{

struct SimulationBufferSegment;
struct SimulationBuffer;

class SimulationBufferPartIterator
{
public:

    SimulationBufferPartIterator( const SimulationBuffer& sb, size_t idx = 0 ) : 
        simulation_buffer( sb ), idx( idx )
    {}

    bool operator!=( const SimulationBufferPartIterator& other )
    {
      return( idx != other.idx );
    }

    const SimulationBufferPartIterator& operator++()
    {
        idx++;
        return *this;
    }

    const SimulationBufferSegment& operator*() const;

private:
    const SimulationBuffer&   simulation_buffer;
    size_t                                  idx;
};


class SimulationBufferPart
{
public:
  SimulationBufferPart( const SimulationBuffer& sb, size_t start, size_t stop ) :
      simulation_buffer( sb ), start( start ), stop( stop )
  {}

  SimulationBufferPartIterator begin() const
  {
      return SimulationBufferPartIterator( simulation_buffer, start );
  }

  SimulationBufferPartIterator end() const
  {
      return SimulationBufferPartIterator( simulation_buffer, stop );
  }

private:
  const SimulationBuffer& simulation_buffer;

  size_t   start, 
           stop;
};

struct SimulationBufferSegment
// One segment of data that grows as needed
{
  std::vector<float>    vertices;  // flattened 3D buffer x,y,z, x,y,z, ...
  Timestamps<float>  time_stamps;  //                     t0,    t1,    t2, ....
  Timestamps<Event>       events;
  // Need a checkpoint object here
  // probably should just be a copy of the spaceship object
   
  void add( float jd, Vector& v )
  {
    vertices.push_back( v.x );
    vertices.push_back( v.y );
    vertices.push_back( v.z );
    time_stamps.push_back( jd );
  }

  // void set_interval( float jd0, float jd1 )
  // {
  //   start = time_stamps.find( jd0 );
  //   stop  = time_stamps.find( jd1 );    
  // }
};

struct SimulationBuffer
// Collection of data segments 
{
  std::vector<SimulationBufferSegment> segments;
  SimulationBufferSegment* last_segment;
  // We keep a pointer to the last segment for convenience

  size_t max_segment_size;
  // When a segment reaches this size, we create a new one.
  size_t changed_from;
  // This is for who ever is shadowing this data buffer.
  // When a new segment is added since the last copy, we
  // set this value.
  // When segments are dumped, because we are re-doing the
  // simulation from a certain segment, we set this value.
  // When it goes to copy the data the shadower then knows
  // from which segment to copy.
  // We only have one shadower, so we use a single variable
  // If we had many we would have created a list and a 
  // register shadower function to manage them

  SimulationBuffer( size_t seg_size=1000 )
  {
    segments.push_back( SimulationBufferSegment() );
    last_segment = &segments.back();
    max_segment_size = seg_size;
    changed_from = 0;
  }

  void add( float jd, Vector& v )
  {
    if( last_segment->time_stamps.size() == max_segment_size )
    {
      segments.push_back( SimulationBufferSegment() );
      last_segment = &segments.back();      
    }
    last_segment->add( jd, v );
  }

  void add_event( Event& e )
  {
    last_segment->events.push_back( e );
  }

  void truncate( float jd )
  // Remove all data from after jd. Segments whose start > jd are simply
  // discarded. The segment whose start < jd and end > jd is truncated to
  // jd. Don't forget to do this for three data structures
  // 1. The vertices
  // 2. The events
  // 3. the time_stamps
  {
    std::runtime_error("Not implemented");
  }

  SimulationBufferPart copy()
  {
    SimulationBufferPart sbp( *this, changed_from, segments.size() );
    changed_from = segments.size();
    return sbp;
  }
};

const SimulationBufferSegment& 
SimulationBufferPartIterator::operator*() const
{
  return simulation_buffer.segments[ idx ];
}




/*
struct CircularDisplayBuffer
// fixed size buffer that efficiently mimics a circular buffer
{
  std::vector<Vector> vertices;
  void add( Vector& v )
  {
    vertices.push_back( v );
  }
};
*/
// struct CircularDisplayBuffer
// // fixed size buffer that efficiently mimics a circular buffer
// {
//   std::vector<GLfloat> vertices;
//   void add( Vector& v )
//   {
//     vertices.push_back( v.x );
//     vertices.push_back( v.y );
//     vertices.push_back( v.z );    
//   }
// };

// typedef std::unordered_map<std::string, DisplayBuffer> dbuf_map_t;
// typedef std::unordered_map<std::string, CircularDisplayBuffer> cdbuf_map_t;

} // namespace sim