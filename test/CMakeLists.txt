#
#   module  : CMakeLists.txt
#   version : 1.5
#   date    : 04/11/22
#
macro(exe9 src)
add_custom_target(${src}.out ALL
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test/${src}.joy
		  COMMAND joy ${CMAKE_SOURCE_DIR}/test/${src}.joy >${src}.out)
endmacro(exe9)

exe9(test01)
exe9(test02)
exe9(test03)
exe9(test06)
exe9(test07)
exe9(test08)
exe9(test09)
exe9(test10)
exe9(test11)
exe9(test12)
exe9(test13)
exe9(test14)
exe9(test15)
exe9(test16)
exe9(test17)
exe9(test18)
exe9(test19)

#
#  Taking parameter 10 and redirecting
#
add_custom_target(test04.out ALL
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test/test04.joy
		  COMMAND joy 10 -vd <${CMAKE_SOURCE_DIR}/test/test04.joy
                          >test04.out)

#
#  Taking parameter test05.joy
#
add_custom_target(test05.out ALL
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test/test05.joy
		  COMMAND joy -vd ${CMAKE_SOURCE_DIR}/test/test05.joy
                          ${CMAKE_SOURCE_DIR}/test/test05.joy >test05.out)
