#
#   module  : CMakeLists.txt
#   version : 1.5
#   date    : 07/03/21
#
cmake_minimum_required(VERSION 3.0)
project(Joy VERSION 1.0)
set(CMAKE_BUILD_TYPE Release)
add_definitions(-DNOBDW)
if(WIN32)
add_definitions(-DGC_NOT_DLL -D_CRT_SECURE_NO_WARNINGS)
else()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage") # debug, no optimization
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage") # enabling coverage
endif()
add_executable(joy interp.c scan.c utils.c main.c srand.c rand.c factor.c gc.c)
add_dependencies(joy always)
add_custom_target(always
		  COMMAND sh banner.sh ${PROJECT_NAME} ${PROJECT_VERSION}
		  COMMAND sh table.sh
		  COMMAND sh prims.sh
		 )
if(WIN32)
else()
target_link_libraries(joy m)
add_subdirectory(lib)
add_subdirectory(test)
endif()
