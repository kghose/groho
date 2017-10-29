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
#include <unordered_map>
//#include <string>

#include "vector.hpp"
#include "ballofmud.hpp"


namespace sim {


// We need a pointer because we will subclass BallOfMud as needed
typedef std::shared_ptr<BallOfMud> sptr_bom_t;
typedef std::shared_ptr<const BallOfMud> sptr_const_bom_t;
typedef std::unordered_map<std::string, sptr_bom_t> map_sp_bom_t;

class Orrery
{
protected:
  map_sp_bom_t body;  // We need a map because we often need to retrieve 
                      // a body by name

public:
  void load( Orrery o ) { body = o.body; }
  void add_body( std::string name, sptr_bom_t p ) { body[name] = p; }
  size_t size() { return body.size(); }
  //sptr_const_bom_t operator[]( int i ) { return body[ i ]; }  
  //sptr_const_bom_t get_body( int i ) { return body[ i ]; }

  void propagate( double jd )
  {
    for( const auto & [ name, b ] : body ) { b->propagate( jd ); }
  }

  const Vector compute_g( const Vector& pos ) 
  {
    Vector g;
    for( const auto & [ name, b ] : body ) 
    {
      Vector this_g = b->pos - pos;
      g += this_g / this_g.norm_sq();
    }
    return g;
  }
};

} // namespace sim
