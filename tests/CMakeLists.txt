cmake_minimum_required( VERSION 3.0.0 )

project( groho_tests )

set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_CXX_FLAGS "-std=c++1z -fsanitize=address -pthread -Wall")
set(CMAKE_CXX_FLAGS "-std=c++1z -pthread -Wall")

# Prepare "Catch" library for other executables
set( CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
add_library( Catch INTERFACE )
target_include_directories( Catch INTERFACE ${CATCH_INCLUDE_DIR} )
include_directories( ../src ../src/external )

# Make test executable
file( GLOB TEST_SOURCES *.cpp )
add_executable( tests ${TEST_SOURCES} )
target_link_libraries( tests Catch )