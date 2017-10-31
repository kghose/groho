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


class Checkpoints : public checkpointvec_t
{  
public:
  // return first index that is on or after given date
  // return checkpoints.size() if past end of data
  inline size_t 
  find( double jd )
  {
    if( size() == 0 )              return 0;
    if( back()->julian_date < jd ) return size();  
    if( (*this)[ 0 ]->julian_date >= jd )  return 0;
    
    size_t i0 = 0
        , i2 = size() - 1
        , i1;
    for(;;)
    {
      if( (*this)[ i0 ]->julian_date == jd ) return i0;
      if( (*this)[ i2 ]->julian_date == jd ) return i2;
      if( i2 == i0 + 1 )                     return i2;

      i1 = ( i2 + i0 ) / 2;
      if( (*this)[ i1 ]->julian_date == jd ) return i1;
      if( (*this)[ i1 ]->julian_date < jd )  { i0 = i1; }
      else                                   { i2 = i1; }
    }
  }

  // discard checkpoints on or after given date
  inline void 
  discard_stale_data( double jd )
  {
    size_t i = this->find( jd );
    if( i == size() ) return;
    resize( i );
  }

};


} // namespace sim