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
  ../src/sampling
  ../src/simulator
  ../src/scenario
  ../src/simulation
)

file(GLOB SOURCES
  ../src/lib/inputfile.cpp
  ../src/orrery/*.cpp
  ../src/sampling/*.cpp 
  ../src/simulator/*.cpp
  ../src/scenario/*.cpp
  ../src/simulation/*.cpp
)


# Make test executable
set( TEST_SOURCES
  main.cpp
  units_test.cpp
  spk_test.cpp
  orrery_test.cpp
  doublebuffer_test.cpp
  inputfile_test.cpp
  sampling_test.cpp
  scenario_test.cpp
  state_test.cpp
)

add_executable( tests ${TEST_SOURCES} ${SOURCES} )
target_link_libraries( 
  tests 
  Catch 
  -ldl # gcc on linux requires this for loguru
  -lpthread # gcc on linux requires this for loguru
)