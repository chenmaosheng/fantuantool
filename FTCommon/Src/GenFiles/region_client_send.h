#ifndef _H_region_client_SEND
#define _H_region_client_SEND

#include "common.h"


struct RegionClientSend
{
    static int32 ClientTimeReq(void* pServer, uint32 iClientTime);
    static int32 RegionChatReq(void* pServer, const char* strMessage);
};

#endif