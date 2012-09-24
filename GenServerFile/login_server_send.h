#ifndef _H_LOGIN_SERVER_SEND
#define _H_LOGIN_SERVER_SEND

#include "common.h"

struct LoginServerSend
{
	static int32 LoginNtf(void* pClient, uint32 iSessionId, const char* strNickName);
};

#endif