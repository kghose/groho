# cmake -DCMAKE_BUILD_TYPE=Debug ..
# cmake -DSANITIZE=ON -DCMAKE_BUILD_TYPE=Debug ..
# cmake -DCMAKE_BUILD_TYPE=Release ..

cmake_minimum_required( VERSION 3.0.0 )
project( groho )

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules")

# cmake -D SANITIZE=ON ..
option(SANITIZE "Sanitize" OFF)
if(SANITIZE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif()

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
message( FATAL_ERROR "Please make an out of source build: create a build directory and invoke cmake from there." )
endif()

set(CMAKE_CXX_COMPILER "c++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
# NDEBUG will suppress some loguru outputs in Release


file(GLOB GROHO_SOURCES
    "src/main.cpp"
    "src/lib/entrypoints.cpp"
    "src/orrery/*.cpp"
#    "src/actions/*.cpp" 
#    "src/sampling/*.cpp" 
#    "src/scenario/*.cpp"
#    "src/simulator/simulator.cpp"
)
add_executable( groho ${GROHO_SOURCES} )

include_directories(
    "src/"
    "src/external" 
    "src/lib"
    "src/orrery"
    "src/simulation"
    "src/plotting"
#    "src/actions" 
#    "src/sampling" 
#    "src/scenario"
#    "src/simulator"
)

target_link_libraries( 
    groho
    -ldl # gcc on linux requires this for loguru
    -lpthread # gcc on linux requires this for loguru
)
