#include "catch.hpp"
#include "simulationbuffer.hpp"


using namespace sim;


TEST_CASE( "Simulation buffer", "[simulationbuffer]" )
{
  SimulationBuffer sb;
  sb.tolerance_sq = 0;
  REQUIRE( sb.size() == 1 );
  
  int sgn = -1;
  for( int i = 0; i < 2 * config::buffer_size; i++ )
  {
    Vector v( i, sgn * i, 0 );
    sgn *= -1;
    sb.add( i, v );
    // we want to make sure the interpolation downsampler triggers at each time
    // step, so we zig-zag the points and set tolerance to 0
  }
  REQUIRE( sb.size() == 1 );

  SECTION( "Adding past the size of a buffer should increase the number of segments" ) {
    Vector v(0, +200, -200);
    sb.add( 1000, v );
    REQUIRE( sb.size() == 2 );

    SECTION( "When we ask to iterate over the segments we should only get "
             " ready segments" ) {
      int ctr = 0;
      for( auto& a : sb ) { ctr++; }
      REQUIRE( ctr == 1 );
    } 
  }

  SECTION( "When we ask to iterate over the segments we should get ready segments "
            " even if it is the last" ) {
    int sgn = -1;
    for( int i = 0; i < config::buffer_size; i++ )
    {
      Vector v( i, sgn * i, 0 );
      sgn *= -1;
      sb.add( i, v );
    }
    REQUIRE( sb.size() == 2 );
    
    int ctr = 0;
    for( auto& a : sb ) { ctr++; }
    REQUIRE( ctr == 1 );

    sb.finalize();
    ctr = 0;
    for( auto& a : sb ) { ctr++; }
    REQUIRE( ctr == 2 );
  } 
}