#ifndef _H_TEST_CLIENT_RECV
#define _H_TEST_CLIENT_RECV

#include "common.h"

struct Test_Client_Recv
{
	// must implement at app layer
	static void LoginReq(void* pClient, const char* strNickname);
};

#endif
