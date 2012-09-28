#ifndef _H_LOGIN_CLIENT_RECV
#define _H_LOGIN_CLIENT_RECV

#include "server_common.h"

struct LoginClientRecv
{
	// must implement at app layer
	static void LoginReq(void* pClient, const char* strNickname);
};

#endif
