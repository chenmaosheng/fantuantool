#ifndef _H_TEST_SERVER_SEND
#define _H_TEST_SERVER_SEND

#include "common.h"

struct Test_Server_Send
{
	static int32 LoginNtf(void* pClient, uint32 iSessionId, const char* strNickName);
};

#endif