#ifndef _H_LOGIN_CLIENT_RECV
#define _H_LOGIN_CLIENT_RECV

#include "server_common.h"

struct LoginClientRecv
{
	static void VersionReq(void* pClient, uint32 iVersion);
};

#endif
