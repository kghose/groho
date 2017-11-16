/*
  A lightweight class inheriting from std::vector<float> that stores an
  array of Julian Date timestamps and supplies a binary search.
*/
#pragma once

#include <vector>


namespace sim
{

template<typename T>
class Timestamps : public std::vector<T>
{  
public:
  // return first index that is on or after given date
  // return timestamps.size() if past end of data
  size_t
  find( float jd )
  {
    // https://stackoverflow.com/a/4643295/2512851
    // fascinating reason why we have to use this->size() or std::vector<T>::size()
    // 
    if( this->size()  ==  0 )  return 0;
    if( this->back()  <  jd )  return this->size();  
    if( this->front() >= jd )  return 0;
    
    size_t i0 = 0,
           i2 = this->size() - 1,
           i1
    ;
    for(;;)
    {
      if( (*this)[ i0 ] == jd ) return i0;
      if( (*this)[ i2 ] == jd ) return i2;
      if( i2 == i0 + 1 )        return i2;

      i1 = ( i2 + i0 ) / 2;
      if( (*this)[ i1 ] == jd ) return i1;
      if( (*this)[ i1 ]  < jd ) { i0 = i1; }
      else                      { i2 = i1; }
    }
  }
};


} // namespace sim