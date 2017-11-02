# mkdir build && cd build && cmake ..
# mkdir xcode_build && cd xcode_build && cmake -G Xcode ..

cmake_minimum_required( VERSION 3.0.0 )
project( groho )

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
message( FATAL_ERROR "Please make an out of source build: create a build directory and invoke cmake from there." )
endif()

#find_package( OpenGL REQUIRED )
#set( ALL_LIBS ${OPENGL_LIBRARY} glfw )
#include_directories( src lib/glfw/include )

find_package( glfw3 3.2 REQUIRED )
find_package( OpenGL REQUIRED )
set( ALL_LIBS ${OPENGL_gl_LIBRARY} glfw )

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++1z -pthread -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
# NDEBUG will suppress some loguru outputs in Release
set(CMAKE_CXX_FLAGS_DEBUG "-g")

file(GLOB SOURCES "src/*.cpp")

set(SOURCE_FILES ${SOURCES})

add_executable( groho ${SOURCE_FILES} )
target_link_libraries( groho ${ALL_LIBS} )