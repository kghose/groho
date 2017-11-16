#include "catch.hpp"

#include "timestamps.hpp"
#include "event.hpp"


using namespace sim;


/*
template<typename T>
void all_tests( 
  const std::string& type_name, 
  Timestamps<T>& empty_v,
  Timestamps<T>& v,
  float min_jd, float max_jd )
{
  SCENARIO( 
    "Timestamps allow us to keep a time series of " + type_name
    " objects and index into it", "[timestamps]" )
  {
    GIVEN( "An empty Timestamps object" ) {
      WHEN( "We look for data" ) {
        double jd = 100;
        size_t idx = empty_v.find( jd );

        THEN( "Our index should always be 0" ) {
          REQUIRE( idx == 0 );
        }
      }
    }

    GIVEN( "A filled out Timestamps object" ) 
    {      
      WHEN( "We look for data after the last timestamp" ) {
        double jd = max_jd + 0.5;
        size_t idx = v.find( jd );

        THEN( "Our index should be size of array" ) {
          REQUIRE( idx == v.size() );
        }
      }
      
      WHEN( "We look for data before the first checkpoint" ) {
        double jd = 0;
        size_t idx = v.find( jd );

        THEN( "Our index should be 0" ) {
          REQUIRE( idx == 0 );
        }
      }
      
      WHEN( "We look for a timestamp after a certain date within our data" ) {
        THEN( "We should get the first checkpoint on or after that date" ) 
        {
          for( double jd = min_jd; jd < max_jd; jd+=0.1 )
          {
            size_t idx = v.find( jd );
            if( idx == v.size() ) 
            {
              REQUIRE( jd > v.back() );            
            }
            else
            {
              if( idx > 0 ) { REQUIRE( v[ idx - 1 ] < jd ); }
              REQUIRE( v[ idx ] >= jd );
              if( idx < v.size() - 1 ) { REQUIRE( v[ idx + 1 ] > jd ); }            
            }
          }
        }
      }
    }
  }
}*/


template<typename T>
void all_tests( 
  const std::string& type_name, 
  Timestamps<T>& empty_v,
  Timestamps<T>& v,
  float min_jd, float max_jd )
{
  SECTION( "'find' on an empty Timestamps object should return 0" ) {
    double jd = 100;
    size_t idx = empty_v.find( jd );
    REQUIRE( 0 == idx );
  }
  
  SECTION( "'find' beyond the last object should return size of the array" ) {
    double jd = max_jd + 0.5;
    size_t idx = v.find( jd );
    REQUIRE( idx == v.size() );
  }
      
  SECTION( "'find' before the first object should return 0" ) {
    double jd = 0;
    size_t idx = v.find( jd );
    REQUIRE( idx == 0 );
  }

  SECTION( "'find' after a certain date should return the first index on or after that date" ){
    for( float jd = min_jd; jd < max_jd; jd+=0.1 )
    {
      size_t idx = v.find( jd );
      if( idx == v.size() ) 
      {
        REQUIRE( jd > v.back() );            
      }
      else
      {
        if( idx > 0 ) { REQUIRE( v[ idx - 1 ] < jd ); }
        REQUIRE( v[ idx ] >= jd );
        if( idx < v.size() - 1 ) { REQUIRE( v[ idx + 1 ] > jd ); }            
      }
    }
  }      
}


TEST_CASE( "Timestamps", "Tests Timestamps with different data types" )
{
  float min_jd = 10.0, max_jd = 100.0; 

  Timestamps<float> empty_vf, vf;
  for( float jd = min_jd; jd < max_jd; jd += 0.5 )
    vf.push_back( jd );
  all_tests<float>( "float", empty_vf, vf, min_jd, max_jd );

  Timestamps<Event> empty_ve, ve;
  for( float jd = min_jd; jd < max_jd; jd += 0.5 )
    ve.push_back( Event( jd ) );
  all_tests<Event>( "float", empty_ve, ve, min_jd, max_jd );  
}
