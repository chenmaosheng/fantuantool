#include "util.h"
#include <windows.h>

uint8 NumberPower(uint32 iNumber)
{
	uint8 i = 0;
	for (; i < 31 && iNumber != 0; ++i)
	{
		iNumber >>= 1;
	}

	return i;
}

int32 Char2WChar(const char* strChar, int32 iCharLen, TCHAR* strTChar, int32 iTCharLen)
{
	return MultiByteToWideChar(CP_UTF8, 0, strChar, iCharLen, strTChar, iTCharLen);
}

int32 Char2WChar(const char* strChar, TCHAR* strTChar, int32 iTCharLen)
{
	return Char2WChar(strChar, -1, strTChar, iTCharLen);
}

int32 WChar2Char(const TCHAR* strTChar, int32 iTCharLen, char* strChar, int32 iCharLen)
{
	return WideCharToMultiByte(CP_UTF8, 0, strTChar, iTCharLen, strChar, iCharLen, NULL, NULL);
}

int32 WChar2Char(const TCHAR* strTChar, char* strChar, int32 iCharLen)
{
	return WChar2Char(strTChar, -1, strChar, iCharLen);
}