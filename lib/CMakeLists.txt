add_custom_target(demo01.out ALL
		  DEPENDS joy
		  COMMAND joy gcd 24 54 >demo01.out)
add_custom_target(demo02.out ALL
		  DEPENDS joy
		  COMMAND joy jp-church.joy >demo02.out)
add_custom_target(demo03.out ALL
		  DEPENDS joy
		  COMMAND joy lsptst.joy >demo03.out)
add_custom_target(demo04.out ALL
		  DEPENDS joy
		  COMMAND joy jp-joytst.joy >demo04.out)
add_custom_target(demo05.out ALL
		  DEPENDS joy
		  COMMAND joy laztst.joy >demo05.out)
add_custom_target(demo06.out ALL
		  DEPENDS joy
		  COMMAND joy grmtst.joy >demo06.out)
add_custom_target(demo07.out ALL
		  DEPENDS joy
		  COMMAND joy modtst.joy >demo07.out)
add_custom_target(demo08.out ALL
		  DEPENDS joy
		  COMMAND joy mtrtst.joy >demo08.out)
add_custom_target(demo09.out ALL
		  DEPENDS joy
		  COMMAND joy plgtst.joy >demo09.out)
add_custom_target(demo10.out ALL
		  DEPENDS joy
		  COMMAND joy symtst.joy >demo10.out)
add_custom_target(demo11.out ALL
		  DEPENDS joy
		  COMMAND joy test.joy >demo11.out)
add_custom_target(demo12.out ALL
		  DEPENDS joy
		  COMMAND joy reptst.joy >demo12.out)
add_custom_target(demo13.out ALL
		  DEPENDS joy
		  COMMAND joy jp-nestrec.joy >demo13.out)
add_custom_target(demo14.out ALL
		  DEPENDS joy
		  COMMAND joy jp-reprodtst.joy >demo14.out)
add_custom_target(demo15.out ALL
		  DEPENDS joy
		  COMMAND joy flatjoy.joy >demo15.out)
