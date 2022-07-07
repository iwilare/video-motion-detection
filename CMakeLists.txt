cmake_minimum_required(VERSION 2.8.12)
project( SPM )
find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED) # Comments this if OpenMP is not needed
include_directories( ${OpenCV_INCLUDE_DIRS} )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# Measurements program
add_executable( measurements src/measurements.cpp )
set_property(TARGET measurements PROPERTY CXX_STANDARD 20)
target_compile_options( measurements PRIVATE -O3 -Wall)
target_link_libraries( measurements ${OpenCV_LIBS})

# Sequential
add_executable( sequential src/sequential.cpp )
set_property(TARGET sequential PROPERTY CXX_STANDARD 20)
target_compile_options( sequential PRIVATE -O3 -Wall)
target_link_libraries( sequential ${OpenCV_LIBS} pthread )

# FastFlow
add_executable( fastflow src/fastflow.cpp )
set_property(TARGET fastflow PROPERTY CXX_STANDARD 20)
target_compile_options( fastflow PRIVATE -O3 -Wall)
target_link_libraries( fastflow ${OpenCV_LIBS} pthread )

# FastFlow blocking
add_executable( fastflow_blocking src/fastflow_blocking.cpp )
set_property(TARGET fastflow_blocking PROPERTY CXX_STANDARD 20)
target_compile_options( fastflow_blocking PRIVATE -O3 -Wall)
target_link_libraries( fastflow_blocking ${OpenCV_LIBS} pthread )

# FastFlow blocking no mapping
add_executable( fastflow_blocking_no_map src/fastflow_blocking_no_map.cpp )
set_property(TARGET fastflow_blocking_no_map PROPERTY CXX_STANDARD 20)
target_compile_options( fastflow_blocking_no_map PRIVATE -O3 -Wall)
target_link_libraries( fastflow_blocking_no_map ${OpenCV_LIBS} pthread )

# Threads
add_executable( threads src/threads.cpp )
set_property(TARGET threads PROPERTY CXX_STANDARD 20)
target_compile_options( threads PRIVATE -O3 -Wall)
target_link_libraries( threads ${OpenCV_LIBS} pthread )

# Threads_pinned
add_executable( threads_pinned src/threads_pinned.cpp )
set_property(TARGET threads_pinned PROPERTY CXX_STANDARD 20)
target_compile_options( threads_pinned PRIVATE -O3 -Wall)
target_link_libraries( threads_pinned ${OpenCV_LIBS} pthread )

# OpenMP
add_executable( omp src/omp.cpp )
set_property(TARGET omp PROPERTY CXX_STANDARD 20)
target_compile_options( omp PRIVATE -fopenmp -O3 -Wall )
target_link_libraries( omp ${OpenCV_LIBS} pthread OpenMP::OpenMP_CXX)
