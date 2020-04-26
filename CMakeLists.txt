cmake_minimum_required(VERSION 3.13)
project(equihash)
set(CMAKE_CXX_STANDARD 11)

SET(GCC_COMPILE_FLAGS "-m64 -maes -mavx -O3")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")
add_executable(equihash solver.cpp main.cpp ./blake/blake2b.cpp proof.cpp utils.cpp proof.h input.h input.cpp)