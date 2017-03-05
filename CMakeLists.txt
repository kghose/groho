# cd build; cmake ..
# cmake -G Xcode ..

cmake_minimum_required(VERSION 3.0.0)
project(groho)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -lpthread")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

include_directories(src)
file(GLOB SOURCES "src/*.cpp")
add_executable(groho ${SOURCES})
