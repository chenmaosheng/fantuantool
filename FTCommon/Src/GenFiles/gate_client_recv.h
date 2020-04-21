#ifndef _H_gate_client_RECV
#define _H_gate_client_RECV

#include "common.h"
#include "ftd_define.h"

struct GateClientRecv
{
    static void AvatarListReq(void* pClient);
    static void AvatarCreateReq(void* pClient, const ftdAvatarCreateData& data);
    static void AvatarSelectReq(void* pClient, const char* strAvatarName);
    static void ChannelSelectReq(void* pClient, const char* strChannelName);
    static void ChannelLeaveReq(void* pClient);
};

#endif