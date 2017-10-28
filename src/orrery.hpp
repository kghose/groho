/*
   Code to manage the "background" elements - planets, moons, larger asteroids -
   that are sources of gravity and whose orbits are predetermined in some
   manner.

   The Orrery will return an immutable list of objects with positions and
   masses and sizes when asked for the state of the universe at some time t.
 */
#pragma once

#include <cmath>
#include <stdlib.h>
#include <vector>
//#include <string>

#include "ballofmud.hpp"


namespace sim {


// We need a pointer because we will subclass BallOfMud as needed
typedef std::shared_ptr<BallOfMud> sptr_bom_t;
typedef std::shared_ptr<const BallOfMud> sptr_const_bom_t;
typedef std::vector<sptr_bom_t> vect_sptr_bom_t;

class Orrery
{
protected:
  vect_sptr_bom_t body;

public:
  void load( Orrery o ) { body = o.body; }
  void add_body( sptr_bom_t p ) { body.push_back( p ); }
  size_t size() { return body.size(); }
  sptr_const_bom_t operator[]( int i ) { return body[ i ]; }  
  sptr_const_bom_t get_body( int i ) { return body[ i ]; }
  void propagate( double jd )
  {
    for(int i = 1 ; i < body.size(); i++ ) { body[ i ]->propagate( jd ); }
  }
};

} // namespace sim
