cmake_minimum_required( VERSION 3.0.0 )

project( groho_tests )

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++1z -fsanitize=address -Wall")
#set(CMAKE_CXX_FLAGS "-std=c++1z -Wall")

# Prepare "Catch" library for other executables
set( CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
add_library( Catch INTERFACE )
target_include_directories( Catch INTERFACE ${CATCH_INCLUDE_DIR} )
include_directories( 
  ../src 
  ../src/opengl 
  ../src/external )

# Make test executable
set( SOURCES 
)
file( GLOB TEST_SOURCES *.cpp )
add_executable( tests ${TEST_SOURCES} ${SOURCES} )
target_link_libraries( tests Catch )