#ifndef _H_login_client_RECV
#define _H_login_client_RECV

#include "common.h"

struct LoginClientRecv
{
    static void VersionReq(void* pClient, uint32 iVersion);
};

#endif