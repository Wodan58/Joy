#
#   module  : CMakeLists.txt
#   version : 1.1
#   date    : 04/11/22
#
macro(exe9 src)
add_custom_target(${src}.out ALL
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/lib/${src}.joy
		  COMMAND joy ${CMAKE_SOURCE_DIR}/lib/${src}.joy 24 54 >${src}.out)
endmacro(exe9)

exe9(gcd)
exe9(jp-church)
exe9(lsptst)
exe9(jp-joytst)
exe9(laztst)
exe9(grmtst)
exe9(modtst)
exe9(mtrtst)
exe9(plgtst)
exe9(symtst)
exe9(test)
exe9(reptst)
exe9(jp-nestrec)
exe9(jp-reprodtst)
exe9(flatjoy)
