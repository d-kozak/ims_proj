cmake_minimum_required(VERSION 3.6)
project(ims_proj)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES basic_model.cpp)
add_executable(ims_proj ${SOURCE_FILES})