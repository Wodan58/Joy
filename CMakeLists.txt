#
#   Makefile for JOY
#
cmake_minimum_required(VERSION 3.0)
project(Joy VERSION 1.0)
set(CMAKE_BUILD_TYPE Release)
if(WIN32)
add_definitions(-DGC_NOT_DLL -D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic")
else()
#add_definitions(-DSTATS)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -g -O0 --coverage -fprofile-arcs -ftest-coverage") # debug, no optimization
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage") # enabling coverage
endif()
add_executable(joy interp.c scan.c utils.c main.c)
if(WIN32)
else()
target_link_libraries(joy m)
add_subdirectory(lib)
add_subdirectory(test)
endif()
