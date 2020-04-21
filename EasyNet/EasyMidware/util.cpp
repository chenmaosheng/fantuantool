#include <windows.h>
#include "type.h"

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