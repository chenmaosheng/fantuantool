#ifndef _H_EASY_HANDLER
#define _H_EASY_HANDLER

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL (CALLBACK *ON_CONNECTION)(ConnID);
typedef void (CALLBACK *ON_DISCONNECT)(ConnID);
typedef void (CALLBACK *ON_DATA)(ConnID, uint32, char*);
typedef void (CALLBACK *ON_CONNECT_FAILED)(void*);

typedef struct _EasyHandler
{
	ON_CONNECTION OnConnection;
	ON_DISCONNECT OnDisconnect;
	ON_DATA OnData;
	ON_CONNECT_FAILED OnConnectFailed;
}EasyHandler;

#ifdef __cplusplus
}
#endif

#endif
