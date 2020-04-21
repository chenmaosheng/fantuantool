#ifndef _H_region_server_RECV
#define _H_region_server_RECV

#include "common.h"

struct RegionServerRecv
{
    static void ServerTimeNtf(void* pClient, uint32 iServerTime);
    static void InitialAvatarDataNtf(void* pClient, uint64 iAvatarId, const char* strAvatarName);
    static void RegionAvatarEnterNtf(void* pClient, uint64 iAvatarId, const char* strAvatarName);
    static void RegionAvatarLeaveNtf(void* pClient, uint64 iAvatarId);
    static void RegionChatNtf(void* pClient, uint64 iAvatarId, const char* strMessage);
};

#endif