#
#   module  : CMakeLists.txt
#   version : 1.2
#   date    : 10/29/24
#
macro(exe9 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy >${src}.out)
endmacro(exe9)

macro(exe8 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy -x ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  	  >${src}.out)
endmacro(exe8)

macro(exe7 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy -t ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  	  >${src}.out)
endmacro(exe7)

macro(exe6 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy -kh ${CMAKE_SOURCE_DIR}/test4/${src}.joy)
endmacro(exe6)

macro(exe5 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy -y ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  	  >${src}.out)
endmacro(exe5)

macro(exe4 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy -adegiluvw ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  	  >${src}.out)
endmacro(exe4)

macro(exe3 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy dummy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  	  >${src}.out)
endmacro(exe3)

macro(exe2 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy <${CMAKE_SOURCE_DIR}/test4/${src}.joy >${src}.out)
endmacro(exe2)

macro(exe1 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy -s ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  	  >${src}.out)
endmacro(exe1)

macro(exe0 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test4
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  COMMAND joy -p ${CMAKE_SOURCE_DIR}/test4/${src}.joy
		  	  >${src}.out)
endmacro(exe0)

exe9(factor)
exe8(gcno)
exe7(interp)
exe6(main1)
exe5(main2)
exe4(main3)
exe3(main4)
exe2(main5)
exe1(main6)
exe9(module1)
exe9(module2)
exe0(scan1)
exe9(scan2)
exe9(symbol)
exe9(forget)
exe9(utils1)
exe9(utils2)
exe9(errors)
