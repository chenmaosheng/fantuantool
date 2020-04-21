#ifndef _H_gate_server_SEND
#define _H_gate_server_SEND

#include "common.h"

#include "ftd_define.h"

struct GateServerSend
{
    static int32 AvatarListAck(void* pServer, int32 iReturn, uint8 iAvatarCount, const ftdAvatar* arrayAvatar);
    static int32 AvatarCreateAck(void* pServer, int32 iReturn, const ftdAvatar& newAvatar);
    static int32 AvatarSelectAck(void* pServer, int32 iReturn, const ftdAvatarSelectData& data);
    static int32 ChannelListNtf(void* pServer, uint8 iChannelCount, const ftdChannelData* arrayChannelData);
    static int32 ChannelSelectAck(void* pServer, int32 iReturn);
    static int32 ChannelLeaveAck(void* pServer);
};

#endif