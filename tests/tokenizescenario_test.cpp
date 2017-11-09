#include "catch.hpp"

#include "tokenizedscenario.hpp"

using namespace sim;


SCENARIO( "A scenario file can contain a header section and/or a program section" ) 
{
  GIVEN( "A file with only a header" ) 
  {
    std::istringstream contents( 
    "name: Earth to mars  # Anything after an octhrope is discarded\n"
    "description: Tutorial example showing how to build an Earth to mars transfer\n"
    "orrery: simplesolarsystem # Can be repeated\n"
    "start: 2458061.5\n"
    "stop:              2458161.5  # spaces are trimmed\n"
    "step: 0.00001\n"
    "flightplan: durga.txt\n"
    "flightplan: durga2.txt  # Can be repeated"
    );
              
    WHEN( "The file is tokenized" ) 
    {
      TokenizedScenario ts;
      contents >> ts;      
      THEN( "We can verify there is only a header" ) 
      {
        REQUIRE( ts.header.size() == 8 );
        REQUIRE( ts.program.size() == 0 );
      }      
    }
  }

  GIVEN( "A file with a header and a program" ) 
  {
    std::istringstream contents( 
    "name: Earth to mars  # Anything after an octhrope is discarded\n"
    "description: Tutorial example showing how to build an Earth to mars transfer\n"
    "orrery: simplesolarsystem # Can be repeated\n"
    "start: 2458061.5\n"
    "stop: 2458161.5\n"
    "step: 0.00001\n"
    "flightplan: durga.txt\n"
    "flightplan: durga2.txt  # Can be repeated\n"
    "\n"
    "234567.34: idle"
    );
              
    WHEN( "The file is tokenized" ) 
    {
      TokenizedScenario ts;
      contents >> ts;      
      THEN( "We can verify that header and program are of correct length" ) 
      {
        REQUIRE( ts.header.size() == 8 );
        REQUIRE( ts.program.size() == 1 );
      }      
    }
  }
}

SCENARIO( "Comments in a scenario file are ignored" ) 
{
  GIVEN( "A file with comments in diferent places" ) 
  {
    std::istringstream contents( 
    "name:       Earth to mars   # Anything after an octhrope is discarded\n"
    "description: Tutorial example showing how to build an Earth to mars transfer\n"
    "orrery: simplesolarsystem # Can be repeated\n"
    "# This whole line is discarded\n"
    "start: 2458061.5\n"
    "# The program section starts here\n"
    "21234: idle # do nothing\n"
    "21235: idle # still do nothing\n"    
    );
              
    WHEN( "The file is tokenized" ) 
    {
      TokenizedScenario ts;
      contents >> ts;      

      THEN( "We can verify the header and program are of the correct length" ) 
      {
        REQUIRE( ts.header.size() == 4 );
        REQUIRE( ts.program.size() == 2 );
      }
      
      THEN( "We can verify comments and leading/trailing spaces have been discarded" ) 
      {
        REQUIRE( ts.header[0].value == "Earth to mars" );
      }
      
    }
  }

}
