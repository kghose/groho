/*
  This handles storing checkpoints
*/
#pragma once

#include <memory>
#include <vector>

namespace sim
{

struct Checkpoint
{
  double julian_date;
  Checkpoint( double jd ) { julian_date = jd; }
};

typedef std::shared_ptr<Checkpoint> checkpointp_t;
typedef std::vector<checkpointp_t> checkpointvec_t;

// return first index that is on or after given date
// return checkpoints.size() if past end of data
inline size_t 
find( double jd, const checkpointvec_t& checkpoints )
{
  if( checkpoints.size() == 0 )              return 0;
  if( checkpoints.back()->julian_date < jd ) return checkpoints.size();  
  if( checkpoints[ 0 ]->julian_date >= jd )  return 0;
  
  size_t i0 = 0
       , i2 = checkpoints.size() - 1
       , i1;
  for(;;)
  {
    if( checkpoints[ i0 ]->julian_date == jd ) return i0;
    if( checkpoints[ i2 ]->julian_date == jd ) return i2;
    if( i2 == i0 + 1 )                         return i2;

    i1 = ( i2 + i0 ) / 2;
    if( checkpoints[ i1 ]->julian_date == jd ) return i1;
    if( checkpoints[ i1 ]->julian_date < jd )  { i0 = i1; }
    else                                       { i2 = i1; }
  }
}

// discard checkpoints on or after given date
inline void 
discard_stale_data( double jd, checkpointvec_t& checkpoints )
{
  size_t i = find( jd, checkpoints );
  if( i == checkpoints.size() ) return;
  checkpoints.resize( i );
}

} // namespace sim