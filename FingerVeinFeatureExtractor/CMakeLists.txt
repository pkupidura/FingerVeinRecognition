cmake_minimum_required(VERSION 3.1)
project(FingerVeinFeatureExtractor)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)

add_executable(FingerVeinFeatureExtractor ${SOURCE_FILES})

target_link_libraries( FingerVeinFeatureExtractor ${OpenCV_LIBS} )