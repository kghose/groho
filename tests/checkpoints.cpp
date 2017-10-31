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


SCENARIO( "We can find data by time in checkpoints", "[checkpoints-find]" ) 
{
  GIVEN( "An empty checkpoint vector" ) 
  {
    checkpointvec_t v;
    WHEN( "We look for data" ) 
    {
      double jd = 100;
      size_t idx = find( jd, v );
      THEN( "Our index should always be 0" ) 
      {
        REQUIRE( idx == 0 );
      }
    }
  }

  GIVEN( "A checkpoint vector with a mixture of Events and SpaceShipState" ) 
  {
    int jd_min = 5, jd_max = 99;

    checkpointvec_t v;
    for( int i = jd_min; i < jd_max; i+=10 )
    {
      v.push_back( checkpointp_t( new Event( i ) ) );   
      v.push_back( checkpointp_t( new SpaceShipState( i + 5 ) ) );   
    }
    
    WHEN( "We look for data after the last checkpoint" ) 
    {
      double jd = 110;
      size_t idx = find( jd, v );

      THEN( "Our index should be size of array" ) 
      {
        REQUIRE( idx == v.size() );
      }
    }
    
    WHEN( "We look for data before the first checkpoint" ) 
    {
      double jd = 0;
      size_t idx = find( jd, v );

      THEN( "Our index should be 0" ) 
      {
        REQUIRE( idx == 0 );
      }
    }
    
    WHEN( "We look for events after a certain date within our data" ) 
    {
      THEN( "We should get the first checkpoint on or after that date" ) 
      {
        for( double jd = jd_min; jd < jd_max; jd++ )
        {
          size_t idx = find( jd, v );
          if( idx == v.size() ) 
          {
            REQUIRE( jd > v.back()->julian_date );            
          }
          else
          {
            if( idx > 0 ) { REQUIRE( v[ idx - 1 ]->julian_date < jd ); }
            REQUIRE( v[ idx ]->julian_date >= jd );
            if( idx < v.size() - 1 ) { REQUIRE( v[ idx + 1 ]->julian_date > jd ); }            
          }
        }
      }
    }
  }
}


SCENARIO( "We can discard checkpoints after a given time", "[checkpoints-discard]" ) 
{
  GIVEN( "A checkpoint vector with a mixture of Events and SpaceShipState" ) 
  {
    int jd_min = 5, jd_max = 99;
    checkpointvec_t v;
    for( int i = jd_min; i < jd_max; i+=10 )
    {
      v.push_back( checkpointp_t( new Event( i ) ) );   
      v.push_back( checkpointp_t( new SpaceShipState( i + 5 ) ) );   
    }
    
    WHEN( "We ask to discard data after a given time point" ) 
    {
      double jd = 53;
      discard_stale_data( jd, v );
      THEN( "Our last data point should end before that time" ) 
      {
        REQUIRE( v.back()->julian_date <= jd );
      }
    }
  }
}