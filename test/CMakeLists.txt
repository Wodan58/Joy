#
#   module  : CMakeLists.txt
#   version : 1.11
#   date    : 08/12/23
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
exe9(test19)
exe9(test20)
exe9(test21)
exe9(test22)
exe9(test25)
exe9(test28)
exe9(test30)
exe9(m0)
exe9(m1)
exe9(m2)
exe9(m3)
exe9(m4)
exe9(m5)
exe9(m6)
exe9(m7)
exe9(m8)
exe9(m9)
exe9(m10)
exe9(m11)
exe9(m12)
exe9(m13)
exe9(m14)
exe9(m15)
exe9(m16)
exe9(m17)
exe9(m18)
exe9(m19)
exe9(m20)
exe9(m21)

#
#  Taking options, parameter 10 and redirecting
#
add_custom_target(test04.out ALL
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test/test04.joy
		  COMMAND joy 10 -vds <${CMAKE_SOURCE_DIR}/test/test04.joy >test04.out)

#
#  Taking filename parameter test05.joy
#
add_custom_target(test05.out ALL
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test/test05.joy
		  COMMAND joy -v ${CMAKE_SOURCE_DIR}/test/test05.joy ${CMAKE_SOURCE_DIR}/test/test05.joy >test05.out)

#
#  Test help
#
add_custom_target(test23.out ALL
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test/test23.joy
		  COMMAND joy -h ${CMAKE_SOURCE_DIR}/test/test23.joy >test23.out)

#
#  Test rest of options
#
add_custom_target(test26.out ALL
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test/test26.joy
		  COMMAND joy -sd <${CMAKE_SOURCE_DIR}/test/test26.joy >test26.out)

add_custom_target(test27.out ALL
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/test/test27.joy
		  COMMAND joy -hs ${CMAKE_SOURCE_DIR}/test/test27.joy >test27.out)

#
#  Test not open file
#
add_custom_target(test24.out ALL
		  DEPENDS joy
		  COMMAND joy nonexist)
