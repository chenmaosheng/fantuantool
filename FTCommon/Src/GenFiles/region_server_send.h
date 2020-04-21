#ifndef _H_region_server_SEND
#define _H_region_server_SEND

#include "common.h"


struct RegionServerSend
{
    static int32 ServerTimeNtf(void* pServer, uint32 iServerTime);
    static int32 InitialAvatarDataNtf(void* pServer, uint64 iAvatarId, const char* strAvatarName);
    static int32 RegionAvatarEnterNtf(void* pServer, uint64 iAvatarId, const char* strAvatarName);
    static int32 RegionAvatarLeaveNtf(void* pServer, uint64 iAvatarId);
    static int32 RegionChatNtf(void* pServer, uint64 iAvatarId, const char* strMessage);
};

#endif