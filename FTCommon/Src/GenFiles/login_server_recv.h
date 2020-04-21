#ifndef _H_login_server_RECV
#define _H_login_server_RECV

#include "common.h"

struct LoginServerRecv
{
    static void VersionAck(void* pClient, int32 iReason);
    static void LoginFailedAck(void* pClient, int32 iReason);
    static void LoginNtf(void* pClient, uint32 iGateIP, uint16 iGatePort);
};

#endif