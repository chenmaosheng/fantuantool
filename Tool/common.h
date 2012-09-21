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
#include <tchar.h>

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

#define SAFE_DELETE(ptr)	if (ptr) {delete (ptr); (ptr) = NULL;}
#define SAFE_DELETE_ARRAY(ptr) if ((ptr) != NULL) { delete [] (ptr); (ptr) = NULL; }

#define MAX_INPUT_BUFFER	1024
#define MAX_OUTPUT_BUFFER	65500

enum
{
	LOG_STARNET,
	LOG_SERVER,

	LOG_TYPE_MAX = LOG_SERVER + 1,
};

#endif