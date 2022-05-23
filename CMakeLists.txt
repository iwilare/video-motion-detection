cmake_minimum_required(VERSION 2.8.12)
project( SPM )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

add_executable( sequential src/sequential.cpp )
target_compile_options( sequential PRIVATE -O3 -Wall )
target_link_libraries( sequential ${OpenCV_LIBS} pthread )

add_executable( fastflow src/fastflow.cpp )
target_compile_options( fastflow PRIVATE -O3 -Wall )
target_link_libraries( fastflow ${OpenCV_LIBS} pthread )
