#ifndef _H_gate_server_RECV
#define _H_gate_server_RECV

#include "common.h"
#include "ftd_define.h"

struct GateServerRecv
{
    static void AvatarListAck(void* pClient, int32 iReturn, uint8 iAvatarCount, const ftdAvatar* arrayAvatar);
    static void AvatarCreateAck(void* pClient, int32 iReturn, const ftdAvatar& newAvatar);
    static void AvatarSelectAck(void* pClient, int32 iReturn, const ftdAvatarSelectData& data);
    static void ChannelListNtf(void* pClient, uint8 iChannelCount, const ftdChannelData* arrayChannelData);
    static void ChannelSelectAck(void* pClient, int32 iReturn);
    static void ChannelLeaveAck(void* pClient);
};

#endif