#ifndef _H_COMMON
#define _H_COMMON

#ifdef __cplusplus
extern "C" {
#endif

#include <tchar.h>
#include <winsock2.h>
#include <mswsock.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

typedef char				int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint32;
typedef __int64				int64;
typedef unsigned __int64	uint64;
typedef float				float32;
typedef double				float64;

typedef HANDLE				ConnID;
#define MAX_BUFFER			1024

#ifdef __cplusplus
}
#endif

#endif
