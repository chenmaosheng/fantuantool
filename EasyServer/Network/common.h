#ifndef _H_COMMON
#define _H_COMMON

#include <winsock2.h>
#include <mswsock.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "type.h"
#include <cstdio>

typedef HANDLE				ConnID;
#define MAX_BUFFER			1024

#define LOG_DBG(Expression, ...)
#define LOG_WAR(Expression, ...) wprintf_s(Expression, __VA_ARGS__)
#define LOG_ERR(Expression, ...) wprintf_s(Expression, __VA_ARGS__)
#define LOG_STT(Expression, ...) wprintf_s(Expression, __VA_ARGS__)

#endif
