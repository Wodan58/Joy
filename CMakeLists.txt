#
#   module  : CMakeLists.txt
#   version : 1.19
#   date    : 10/02/23
#
cmake_minimum_required(VERSION 3.0)
project(Joy VERSION 1.0)
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release")
endif()
option(RUN_TESTS "Run standard tests" OFF)
add_definitions(-DNOBDW -DCOPYRIGHT -DJVERSION="NOBDW ${CMAKE_BUILD_TYPE} ${CMAKE_PROJECT_VERSION}")
include_directories(.)
add_executable(joy interp.c scan.c utils.c main.c factor.c module.c gc.c)
add_dependencies(joy always)
add_custom_target(always
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		  COMMAND sh table.sh .
		  COMMAND sh prims.sh .)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_definitions(-DGC_NOT_DLL -D_CRT_SECURE_NO_WARNINGS)
# target_link_libraries(joy gc-lib)
# include_directories(bdwgc/include)
# add_subdirectory(bdwgc)
else()
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage") # debug, no optimization
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage") # enabling coverage
  endif()
  target_link_libraries(joy m)
  if(RUN_TESTS)
    add_subdirectory(lib)
    add_subdirectory(test)
    add_subdirectory(test2)
  endif()
endif()
