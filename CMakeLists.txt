cmake_minimum_required(VERSION 2.8)
project (filestats)
SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic -fPIC")

add_library(filestatistics SHARED filestatistics.cpp 
								  io.cpp
								  files.cpp)
								  
TARGET_LINK_LIBRARIES(filestatistics dl rt)