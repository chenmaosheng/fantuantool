#ifndef _H_LOGIN_SERVER_SEND
#define _H_LOGIN_SERVER_SEND

#include "server_common.h"

struct LoginServerSend
{
	static int32 LoginNtf(void* pClient, uint32 iGateIP, uint16 iGatePort);
};

#endif