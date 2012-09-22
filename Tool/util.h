#ifndef _H_UTIL
#define _H_UTIL

#include "common.h"

static uint8 NumberPower(uint32 iNumber)
{
	uint8 i = 0;
	for (; i < 31 && iNumber != 0; ++i)
	{
		iNumber >>= 1;
	}

	return i;
}

#endif
