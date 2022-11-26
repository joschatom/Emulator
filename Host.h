#pragma once
#include "types.h"
#include "Opcodes.h"
#include <stdio.h>

#define EXTERNAL_RUNTINE(__code__) {return EXTERNAL_RUNTINE_CODE(__code__);}
#define EXTERNAL_RUNTINE_CODE(__code__) (-##__code__)


int HandleHostcall(u32 ID, void* Paramters[]) {
	switch (ID)
	{
	case HEX(46):
		printf("Host Print Requested!\n");

		printf("DATA:{A=%x;X=%x;Y=%x}\n", Paramters[0], Paramters[1], Paramters[2]);
	case 0x75:EXTERNAL_RUNTINE(75) break;
	case 0x30:EXTERNAL_RUNTINE(30) break;

	default:
		break;
	}

	return 0;
}