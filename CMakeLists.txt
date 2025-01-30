cmake_minimum_required(VERSION 3.10)
project(HighPerformanceCache)

set(CMAKE_CXX_STANDARD 17)

add_executable(cache_app src/main.cpp)
target_include_directories(cache_app PRIVATE include)