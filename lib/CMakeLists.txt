#
#   module  : CMakeLists.txt
#   version : 1.3
#   date    : 11/05/23
#
macro(exe9 src)
add_custom_target(${src}.out ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/lib/${src}.joy
		  COMMAND joy ${CMAKE_SOURCE_DIR}/lib/${src}.joy 24 54 >${src}.out)
endmacro(exe9)

exe9(alljoy)
exe9(flatjoy)
exe9(gcd)
exe9(grmtst)
exe9(jp-church)
exe9(jp-joytst)
exe9(jp-nestrec)
exe9(jp-reprodtst)
exe9(laztst)
exe9(lsptst)
exe9(mandel)
exe9(modtst)
exe9(mtrtst)
exe9(plgtst)
exe9(reptst)
exe9(symtst)
exe9(test)
exe9(tutinp)
