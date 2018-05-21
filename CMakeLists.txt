# cmake -DCMAKE_BUILD_TYPE=Debug ..
# cmake -DSANITIZE=ON -DCMAKE_BUILD_TYPE=Debug ..
# cmake -DCMAKE_BUILD_TYPE=Release ..

cmake_minimum_required( VERSION 3.0.0 )
project( groho )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules")


# cmake -D SANITIZE=ON ..
option(SANITIZE "Sanitize" OFF)
if(SANITIZE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif()

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
message( FATAL_ERROR "Please make an out of source build: create a build directory and invoke cmake from there." )
endif()

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-std=c++1z -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
# NDEBUG will suppress some loguru outputs in Release

find_package( 
    Magnum 
    REQUIRED 
    GL
    Shaders
    Sdl2Application 
)

file(GLOB SOURCES 
    "src/*.cpp" 
    "src/buffer/*.cpp" 
    "src/orrery/*.cpp"
    "src/scenario/*.cpp"
    "src/magnumapp/*.cpp"
)

set(SOURCE_FILES ${SOURCES})
add_executable( groho ${SOURCE_FILES} )

include_directories(
    "src/"
    "src/buffer" 
    "src/orrery"
    "src/scenario"
    "src/magnumapp"
    "src/external" 
)

target_link_libraries( 
    groho
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::Shaders
)
