#ifndef _H_login_client_SEND
#define _H_login_client_SEND

#include "common.h"


struct LoginClientSend
{
    static int32 VersionReq(void* pServer, uint32 iVersion);
};

#endif