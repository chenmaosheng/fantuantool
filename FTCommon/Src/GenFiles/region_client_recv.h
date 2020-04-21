#ifndef _H_region_client_RECV
#define _H_region_client_RECV

#include "common.h"

struct RegionClientRecv
{
    static void ClientTimeReq(void* pClient, uint32 iClientTime);
    static void RegionChatReq(void* pClient, const char* strMessage);
};

#endif