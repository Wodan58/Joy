#
#   module  : CMakeLists.txt
#   version : 1.1
#   date    : 03/21/24
#
macro(exe9 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test1
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test1/${src}.joy
		  COMMAND joy ${CMAKE_SOURCE_DIR}/test1/${src}.joy >${src}.out)
endmacro(exe9)

# test most options
macro(exe8 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test1
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test1/${src}.joy
		  COMMAND joy -a2de3g1lpu1 ${CMAKE_SOURCE_DIR}/test1/${src}.joy >${src}.out)
endmacro(exe8)

# test rest of options
macro(exe7 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test1
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test1/${src}.joy
		  COMMAND joy -hiwx ${CMAKE_SOURCE_DIR}/test1/${src}.joy >${src}.out)
endmacro(exe7)

# test unknown options
macro(exe6 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test1
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test1/${src}.joy
		  COMMAND joy -z ${CMAKE_SOURCE_DIR}/test1/${src}.joy dummy >${src}.out)
endmacro(exe6)

# test tracing and symbol table
macro(exe5 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test1
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test1/${src}.joy
		  COMMAND joy -st ${CMAKE_SOURCE_DIR}/test1/${src}.joy dummy >${src}.out)
endmacro(exe5)

#test access to nonexisting file
macro(exe4 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test1
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test1/${src}.joy
		  COMMAND joy dummy ${CMAKE_SOURCE_DIR}/test1/${src}.joy >${src}.out)
endmacro(exe4)

exe4(exist)
exe7(empty)
exe6(minim)
exe5(tut)
exe8(scan)
exe9(parm)
exe9(const)
exe9(quad)
exe9(incl)
exe9(m20)
exe9(m21)
exe9(m22)
exe9(m23)
exe9(m24)
exe9(m25)
