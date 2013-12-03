cmake_minimum_required(VERSION 2.8)
project (filestats)
SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic")
add_library(filestatistics SHARED filestatistics.cpp)