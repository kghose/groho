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

set(CMAKE_CXX_COMPILER "c++")
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
# NDEBUG will suppress some loguru outputs in Release

find_package( Magnum REQUIRED 
    GL
    MeshTools
    Primitives
    Shaders
    Text
    Sdl2Application
)

find_package( Corrade REQUIRED
    Containers
)

file(GLOB SOURCES 
    "src/*.cpp" 
    "src/actions/*.cpp" 
    "src/buffer/*.cpp" 
    "src/orrery/*.cpp"
    "src/scenario/*.cpp"
    "src/simulator/*.cpp"
    "src/magnumapp/*.cpp"
)

set(SOURCE_FILES ${SOURCES})
corrade_add_resource(RESOURCES resources.conf)
add_executable( groho ${SOURCE_FILES} ${RESOURCES} )

include_directories(
    "src/"
    "src/actions" 
    "src/buffer" 
    "src/orrery"
    "src/scenario"
    "src/simulator"
    "src/magnumapp"
    "src/external" 
)

target_link_libraries( 
    groho
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::Shaders
    Magnum::Text
    -lpthread # gcc on linux requires this
)
