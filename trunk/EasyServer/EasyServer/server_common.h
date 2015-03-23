#ifndef _H_SERVER_COMMON
#define _H_SERVER_COMMON

#include "common.h"
#include "dispatcher.h"

static Dispatcher::Func g_Dispatcherfunc[] = {NULL,};
static int32 g_DispatcherFuncIndex = 1;


#define SERVER_RECV_MESSAGE_1(funcName, P1)				\
	extern void funcName(void* pClient, P1 p1) {		\
		EasySession* pSession = (EasySession*)pClient;	\
		pSession->OnfuncName##(p1);						\
	}													\
	bool CALLBACK funcName##_Callback(void* pClient, InputStream& stream) { \
		P1 p1;											\
		if (!stream.Serialize(p1)) return false;		\
		funcName(pClient, p1);							\
		return true;
	}													\
	
	g_Dispatcherfunc[g_DispatcherFuncIndex++] = funcName##_Callback;	\



#endif
