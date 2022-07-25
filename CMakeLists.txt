#
#   module  : CMakeLists.txt
#   version : 1.12
#   date    : 07/25/22
#
cmake_minimum_required(VERSION 3.0)
project(Joy VERSION 1.0)
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
set(CMAKE_BUILD_TYPE "Debug")
endif()
if(WIN32)
option(WINDOWS "Use _MSC_VER compiler" ON)
endif()
option(RUN_TESTS "Run standard tests" ON)
#add_definitions(-ansi -pedantic "-Dkh_inline=""" -DULLONG_MAX=ULONG_MAX)
add_definitions(-DNOBDW -DTRACING -DJVERSION="NOBDW ${CMAKE_BUILD_TYPE} ${CMAKE_PROJECT_VERSION}")
if(WINDOWS)
add_definitions(-DGC_NOT_DLL -D_CRT_SECURE_NO_WARNINGS)
else()
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage") # debug, no optimization
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage") # enabling coverage
endif()
endif()
include_directories(.)
add_executable(joy interp.c scan.c utils.c main.c factor.c module.c gc.c)
if(WINDOWS)
add_dependencies(joy always)
add_custom_target(always
		  COMMAND sh ${CMAKE_SOURCE_DIR}/table.sh ${CMAKE_SOURCE_DIR}
		  COMMAND sh ${CMAKE_SOURCE_DIR}/prims.sh ${CMAKE_SOURCE_DIR}
		 )
else()
target_link_libraries(joy m)
add_dependencies(joy always)
add_custom_target(always
		  COMMAND sh ${CMAKE_SOURCE_DIR}/banner.sh ${PROJECT_NAME} ${PROJECT_VERSION}
		  COMMAND sh ${CMAKE_SOURCE_DIR}/table.sh ${CMAKE_SOURCE_DIR}
		  COMMAND sh ${CMAKE_SOURCE_DIR}/prims.sh ${CMAKE_SOURCE_DIR}
		 )
if(RUN_TESTS)
#add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(test2)
endif()
endif()
