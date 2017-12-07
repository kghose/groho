#include "simulationobjectview.hpp"


namespace sim
{

size_t 
SimulationObjectView::max_size()
// when we transform obj relative to ref, because of adpative downsampling we
// don't know ahead of time the exact number of points the downsampled data will
// take, but we do know the maximum possible.
{
  if( ref == nullptr ) return obj->path.size();
  else return obj->path.size() + ref->path.size();
}

size_t 
SimulationObjectView::path_view( float* buf, float* t_buf, size_t max_idx )
// Transform obj relative to ref and write out into buf
// write timestamps into t_buf
// return the actual size written
{
  if( ref == nullptr ) return copy( buf, t_buf, max_idx );
  else return transform( buf, t_buf, max_idx );  
}

size_t 
SimulationObjectView::copy( float* buf, float* t_buf, size_t max_idx )
// Write the path of a into a flat buffer that could be from OpenGL
{
  for( int i = 0; i < max_idx; i++ )
  {
    const Vector& p = obj->path[ i ]; 
    buf  [ 3 * i     ] = p.x;
    buf  [ 3 * i + 1 ] = p.y;
    buf  [ 3 * i + 2 ] = p.z;
    t_buf[     i     ] = p.t;
  }
  return max_idx;
}

size_t 
SimulationObjectView::transform( float* buf, float* t_buf, size_t max_idx )
// Given two simulationobjects A and B, transform A such that it is in the
// reference frame formed by B. Write the data into a flat buffer that could
// be from OpenGL
{
  for( int i = 0; i < max_idx; i++ )
  {
    const Vector& p = obj->path[ i ]; 
    buf  [ 3 * i     ] = p.x;
    buf  [ 3 * i + 1 ] = p.y;
    buf  [ 3 * i + 2 ] = p.z;
    t_buf[     i     ] = p.t;
  }
  return max_idx;
}


/*


size_t 
Path::num_points_in_new_frame( Path& new_frame )
{
  if( new_frame.name == reference ) return  size();  // identity transform
  else return            new_frame.size() + size();  // conservative size hint
}

void 
Path::view_in_frame( Path& new_frame, PathView& dv )
{
  assert( dv.name == name );
  dv.reference = new_frame.name;

  dv.allocate( num_points_in_new_frame( new_frame ) );

  if( new_frame.name == reference ) { view_in_existing_frame( dv ); }
  else                              { view_in_new_frame( new_frame, dv ); }
    
  dv.finalize();
}

void
Path::view_in_new_frame( Path& new_frame, PathView& dv )
{
  dv.num_points = 0;
  size_t  ref_idx = 0;
  size_t  ref_size = new_frame.size();
  size_t  data_idx = 0;
  size_t  data_size = size();

  Vector v_new, v_old, v_ref;
  float t;
  while( ( ref_idx < ref_size - 1 ) |
         ( data_idx < data_size ) )
  {
    if ( trajectory[ data_idx ].t < new_frame.trajectory[ ref_idx + 1 ].t )
    {
      t = trajectory[ data_idx ].t;
      v_old = trajectory[ data_idx ];
      v_ref = interpolate( new_frame.trajectory, ref_idx, t );
      data_idx++;
    }
    else
    {
      if ( trajectory[ data_idx ].t > new_frame.trajectory[ ref_idx + 1 ].t )
      {
        t = new_frame.trajectory[ ref_idx ].t;
        v_old = interpolate( trajectory, data_idx - 1, t );
        v_ref = new_frame.trajectory[ ref_idx + 1 ];
        ref_idx++;      
      }
      else
      {
        t = new_frame.trajectory[ ref_idx ].t;
        v_old = trajectory[ data_idx ];
        v_ref = new_frame.trajectory[ ref_idx + 1 ];
        data_idx++;
        ref_idx++;              
      }
    }

    v_new = v_old - v_ref;

    dv.buf  [ 3 * dv.num_points     ] = v_new.x;
    dv.buf  [ 3 * dv.num_points + 1 ] = v_new.y;
    dv.buf  [ 3 * dv.num_points + 2 ] = v_new.z;
    dv.t_buf[     dv.num_points     ] = t;
    dv.num_points++;
  }  
}



Vector 
Path::interpolate( vector_vec_t& p, size_t n0, float t )
{
  return ( p[ n0 + 1 ] - p[ n0 ] ) * ( t - p[ n0 ].t ) / ( p[ n0 + 1 ].t - p[ n0 ].t ) + p[ n0 ];
}
*/

} // namespace sim
