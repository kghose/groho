#include "simulationbuffer.hpp"


namespace sim
{

size_t 
Path::num_points_in_new_frame( Path& ref )
{
  if( reference_frame = ref.name) return size();
  else return size() + ref.size(); // conservative size hint
}

void 
Path::copy_to_new_frame( SimulationData& ref, DataView& dv )
{
  if( reference_frame = ref.name ) return flatten( dv );

  size_t  index = 0;
  size_t  ref_idx = 0;
  size_t  ref_size = ref.size();
  size_t  data_idx = 0;
  size_t  data_size = size();

  Vector v_new, v_old, v_ref;
  float t;
  while( ( ref_idx < ref_size - 1 ) |
         ( data_idx < data_size ) )
  {
    if ( trajectory[ data_idx ].t < ref.trajectory[ ref_idx + 1 ].t )
    {
      t = trajectory[ data_idx ].t
      v_ref = interpolate( ref.trajectory, ref_idx, t );
      v_old = trajectory[ data_idx ];
      data_idx++;
    }
    else
    {
      t = ref.trajectory[ ref_idx ].t
      v_old = interpolate( trajectory, data_idx - 1, t );
      v_ref = ref.trajectory[ ref_idx + 1 ];
      ref_idx++;      
    }

    v_new = v_old - v_ref;

    dv.buf  [ 3 * index     ] = v_new.x;
    dv.buf  [ 3 * index + 1 ] = v_new.y;
    dv.buf  [ 3 * index + 2 ] = v_new.z;
    dv.t_buf[     index     ] = t;
    index++;
  }
  
  dv.n_points = index;
}

void
Path::flatten( DataView& dv )
{
  size_t max_index = size();
  for(size_t index = 0; index < max_index; index++ )
  {
    dv.buf  [ 3 * index     ] = trajectory[ index ].x;
    dv.buf  [ 3 * index + 1 ] = trajectory[ index ].y;
    dv.buf  [ 3 * index + 2 ] = trajectory[ index ].z;
    dv.t_buf[     index     ] = trajectory[ index ].t;
  }
  dv.n_points = max_index;
}

Vector 
Path::interpolate( vector_vec_t& p, size_t n0, float t )
{
  return ( p[ n0 + 1 ] - p[ n0 ] ) * ( t - p[ n0 ].t ) / ( p[ n0 + 1 ].t - p[ n0 ].t ) + p[ n0 ];
}


} // namespace sim
