#include "catch.hpp"

#include "simulationbuffer.hpp"


using namespace sim;


TEST_CASE( "Simulation buffer", "[simulationbuffer]" )
{
  SECTION( "Adding to a buffer should eventually increase the number of segments" ) {
    SimulationBuffer sb( 10 );
    REQUIRE( sb.segments.size() == 1 );
    REQUIRE( sb.changed_from == 0 );
    
    for( int i = 0; i < 10; i++ )
    {
      Vector v(0, 0, i);
      sb.add( i, v );
    }
    REQUIRE( sb.segments.size() == 1 );
    REQUIRE( sb.changed_from == 0 );

    Vector v(0, 0, 11);
    sb.add( 11, v );
    REQUIRE( sb.segments.size() == 2 );    
    REQUIRE( sb.changed_from == 0 );
  }

  SECTION( "Copying from a buffer should update `changed_from`" ) {
    SimulationBuffer sb( 10 );
    for( int i = 0; i < 30; i++ )
    {
      Vector v(0, 0, i);
      sb.add( i, v );
    }
    REQUIRE( sb.segments.size() == 3 );
    REQUIRE( sb.changed_from == 0 );
    
    sb.copy();
    REQUIRE( sb.changed_from == 3 );    
  }

  SECTION( "Copying from a buffer should allow us to iterate over SimulationBufferSegment" ) {
    SimulationBuffer sb( 10 );
    for( int i = 0; i < 30; i++ )
    {
      Vector v(0, 0, i);
      sb.add( i, v );
    }
    REQUIRE( sb.segments.size() == 3 );
    REQUIRE( sb.changed_from == 0 );
    
    int i = 0;
    for( auto& sbs : sb.copy() )
    {
      REQUIRE( sbs.time_stamps.size() == 10 );
      REQUIRE( sbs.time_stamps.front() == 10 * i );
      REQUIRE( sbs.time_stamps.back() == 10 * i + 9 );
      i++;      
    }
    REQUIRE( sb.changed_from == 3 );    
  }
}