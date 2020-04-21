#ifndef _H_gate_client_SEND
#define _H_gate_client_SEND

#include "common.h"

#include "ftd_define.h"

struct GateClientSend
{
    static int32 AvatarListReq(void* pServer);
    static int32 AvatarCreateReq(void* pServer, const ftdAvatarCreateData& data);
    static int32 AvatarSelectReq(void* pServer, const char* strAvatarName);
    static int32 ChannelSelectReq(void* pServer, const char* strChannelName);
    static int32 ChannelLeaveReq(void* pServer);
};

#endif