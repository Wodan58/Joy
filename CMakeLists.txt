#
#   module  : CMakeLists.txt
#   version : 1.39
#   date    : 02/09/26
#
cmake_minimum_required(VERSION 3.28)
project(Joy VERSION 1.0)
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release")
endif()
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  option(RUN_TESTS "Run standard tests" ON)
else()
  option(RUN_TESTS "Run standard tests" OFF)
endif()
add_executable(joy main.c interp.c scan.c utils.c factor.c module.c optable.c
	       symbol.c undefs.c setraw.c repl.c write.c error.c print.c gc.c)
add_definitions(-DLINK="\\"${CMAKE_EXE_LINKER_FLAGS}\\"")
add_definitions(-DVERS="NOBDW ${CMAKE_BUILD_TYPE} ${CMAKE_PROJECT_VERSION}")
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_VERBOSE_MAKEFILE ON)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNOBDW -D_CRT_SECURE_NO_DEPRECATE")
  add_definitions(-DCOMP="\\"${CMAKE_C_FLAGS}\\"")
else()
  add_dependencies(joy always)
  add_custom_target(always
		    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		    COMMAND sh prims.sh .
		    COMMAND sh table.sh .)
  set(CF "-DNOBDW -I.. -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-result")
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CF}")
    add_definitions(-DCOMP="\\"${CMAKE_C_FLAGS_RELEASE}\\"")
  else()
    set(CMAKE_C_FLAGS_DEBUG	# debug, enabling coverage
  "${CMAKE_C_FLAGS_DEBUG} ${CF} -O0 --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    add_definitions(-DCOMP="\\"${CMAKE_C_FLAGS_DEBUG}\\"")
  endif()
  target_link_libraries(joy m)
  if(RUN_TESTS)
    add_subdirectory(lib)
    add_subdirectory(test2)
    add_subdirectory(test4)
  endif()
endif()
