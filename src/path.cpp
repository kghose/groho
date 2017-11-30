#include "path.hpp"


namespace sim
{

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


void
Path::view_in_existing_frame( PathView& dv )
{
  size_t max_idx = size();
  for( dv.num_points = 0; dv.num_points < max_idx; dv.num_points++ )
  {
    dv.buf  [ 3 * dv.num_points     ] = trajectory[ dv.num_points ].x;
    dv.buf  [ 3 * dv.num_points + 1 ] = trajectory[ dv.num_points ].y;
    dv.buf  [ 3 * dv.num_points + 2 ] = trajectory[ dv.num_points ].z;
    dv.t_buf[     dv.num_points     ] = trajectory[ dv.num_points ].t;
  }
}

Vector 
Path::interpolate( vector_vec_t& p, size_t n0, float t )
{
  return ( p[ n0 + 1 ] - p[ n0 ] ) * ( t - p[ n0 ].t ) / ( p[ n0 + 1 ].t - p[ n0 ].t ) + p[ n0 ];
}


} // namespace sim
