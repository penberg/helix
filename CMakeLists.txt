cmake_minimum_required(VERSION 2.8)

project(helix)

set(CMAKE_CFLAGS "-Iinclude -Wall -O3 -g -std=c11")
set(CMAKE_CXX_FLAGS "-Iinclude -Wall -O3 -g -std=c++14")

add_library(helix helix.cc)
target_include_directories(helix PUBLIC include)

add_executable(helix-trace tools/helix-trace/helix-trace.c)
target_include_directories(helix-trace PUBLIC include)
target_link_libraries(helix-trace LINK_PUBLIC helix)
target_link_libraries(helix-trace PUBLIC uv)
