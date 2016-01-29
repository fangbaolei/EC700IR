/*
 * boot.c
 *
 *  Created on: 2009-9-6
 *      Author: Administrator
 */
#include <std.h>
#include <stdlib.h>

#pragma CODE_SECTION(sysReset,".sysReset")

void sysReset(){
	c_int00();
}
