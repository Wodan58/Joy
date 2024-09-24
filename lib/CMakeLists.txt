#
#   module  : CMakeLists.txt
#   version : 1.7
#   date    : 09/24/24
#
macro(exe9 src)
add_custom_target(${src}.old ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/lib/${src}.joy
		  COMMAND joy ${CMAKE_SOURCE_DIR}/lib/${src}.joy 24 54 >${src}.old)
endmacro(exe9)

add_custom_target(tutinp.old ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/lib/tutinp.joy
		  COMMAND joy <${CMAKE_SOURCE_DIR}/lib/tutinp.joy >tutinp.old)

add_custom_target(lsptst.old ALL
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
		  DEPENDS joy ${CMAKE_SOURCE_DIR}/lib/lsptst.joy
		  COMMAND joy <${CMAKE_SOURCE_DIR}/lib/lsptst.joy >tutinp.old)

exe9(jp-joytst)
exe9(laztst)
exe9(symtst)
exe9(plgtst)
exe9(mtrtst)
exe9(grmtst)
exe9(reptst)
exe9(jp-reprodtst)
exe9(flatjoy)
exe9(modtst)
exe9(alljoy)
exe9(gcd)
exe9(jp-church)
exe9(jp-nestrec)
exe9(mandel)
exe9(test)

if(RUN_PACK MATCHES "ON")
set(JOY_LIB agglib.joy fraclib.joy grmlib.joy inilib.joy lazlib.joy lsplib.joy
    mthlib.joy mtrlib.joy numlib.joy plglib.joy replib.joy seqlib.joy
    symlib.joy tutlib.joy typlib.joy)
install(FILES ${JOY_LIB} DESTINATION LIB)
endif()
