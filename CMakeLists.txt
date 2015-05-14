cmake_minimum_required(VERSION 3.2)
project(write-to-bmp)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++11)

set(SOURCE_FILES src/main)

add_executable(write-to-bmp ${SOURCE_FILES})
