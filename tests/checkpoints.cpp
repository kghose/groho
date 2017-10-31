#include "catch.hpp"

#include "event.hpp"
#include "spaceshipstate.hpp"

using namespace sim;


SCENARIO( "Checkpoints are a mixture of SpaceshipState and Events", "[checkpoints]" ) 
{
  
  GIVEN( "A an empty checkpoint vector" ) 
  {
    checkpointvec_t v;
          
    REQUIRE( v.size() == 0 );
          
    WHEN( "Events and SpaceshipStates are inserted" ) 
    {
      v.push_back( checkpointp_t( new Event( 100.0 ) ) );   
      v.push_back( checkpointp_t( new SpaceShipState( 110.0 ) ) );   
      v.push_back( checkpointp_t( new Event( 120.0 ) ) );   
      
      THEN( "We can retrieve them" ) 
      {
          REQUIRE( v[0]->julian_date == 100.0 );
          REQUIRE( v[2]->julian_date == 120.0 );
      }
    }
  }
}