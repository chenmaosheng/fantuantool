#ifndef _H_login_server_SEND
#define _H_login_server_SEND

#include "common.h"


struct LoginServerSend
{
    static int32 VersionAck(void* pServer, int32 iReason);
    static int32 LoginFailedAck(void* pServer, int32 iReason);
    static int32 LoginNtf(void* pServer, uint32 iGateIP, uint16 iGatePort);
};

#endif