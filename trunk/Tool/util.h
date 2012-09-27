/*****************************************************************************************
	filename:	util.h
	created:	09/27/2012
	author:		chen
	purpose:	necessary utility functions

*****************************************************************************************/

#ifndef _H_UTIL
#define _H_UTIL

#include "type.h"

#define SAFE_DELETE(ptr)	if (ptr) {delete (ptr); (ptr) = NULL;}
#define SAFE_DELETE_ARRAY(ptr) if ((ptr) != NULL) { delete [] (ptr); (ptr) = NULL; }

// calculate the 2nd power of a certain number
uint8 NumberPower(uint32 iNumber);

// multibyte and unicode transfer
int32 Char2WChar(const char*, int32, TCHAR*, int32);
int32 Char2WChar(const char*, TCHAR*, int32);
int32 WChar2Char(const TCHAR*, int32, char*, int32);
int32 WChar2Char(const TCHAR*, char*, int32);

#endif
