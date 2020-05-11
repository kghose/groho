cmake_minimum_required( VERSION 3.0.0 )

project( groho_tests )

set(CMAKE_CXX_COMPILER "c++")
set(CMAKE_CXX_FLAGS "-g -std=c++17 -fsanitize=address -Wall")


# Prepare "Catch" library for other executables
set( CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
add_library( Catch INTERFACE )
target_include_directories( Catch INTERFACE ${CATCH_INCLUDE_DIR} )
include_directories( 
  ../src
  ../src/lib 
  ../src/external
  ../src/orrery
  ../src/scenario
)

# Make test executable
set( TEST_SOURCES
  main.cpp
  units_test.cpp
  spk_test.cpp
  inputfile_test.cpp
  scenario_test.cpp
  flightplan_test.cpp
)

set( SOURCES
  ../src/orrery/spk.cpp
  ../src/lib/inputfile.cpp
  ../src/scenario/scenario.cpp
  ../src/scenario/flightplan.cpp
)

add_executable( tests ${TEST_SOURCES} ${SOURCES} )
target_link_libraries( 
  tests 
  Catch 
  -ldl # gcc on linux requires this for loguru
  -lpthread # gcc on linux requires this for loguru
)