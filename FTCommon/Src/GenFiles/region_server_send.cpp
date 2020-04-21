#include "region_server_send.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

int32 RegionServerSend::ServerTimeNtf(void* pServer, uint32 iServerTime)
{
    OutputStream stream;
    if (!stream.Serialize(iServerTime)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_REGION<<8) | 0, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 RegionServerSend::InitialAvatarDataNtf(void* pServer, uint64 iAvatarId, const char* strAvatarName)
{
    OutputStream stream;
    uint16 iLength;
    if (!stream.Serialize(iAvatarId)) return -1;
    iLength = (uint16)strlen(strAvatarName);
    if (!stream.Serialize(iLength)) return -1;
    if (!stream.Serialize(iLength, strAvatarName)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_REGION<<8) | 1, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 RegionServerSend::RegionAvatarEnterNtf(void* pServer, uint64 iAvatarId, const char* strAvatarName)
{
    OutputStream stream;
    uint16 iLength;
    if (!stream.Serialize(iAvatarId)) return -1;
    iLength = (uint16)strlen(strAvatarName);
    if (!stream.Serialize(iLength)) return -1;
    if (!stream.Serialize(iLength, strAvatarName)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_REGION<<8) | 2, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 RegionServerSend::RegionAvatarLeaveNtf(void* pServer, uint64 iAvatarId)
{
    OutputStream stream;
    if (!stream.Serialize(iAvatarId)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_REGION<<8) | 3, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 RegionServerSend::RegionChatNtf(void* pServer, uint64 iAvatarId, const char* strMessage)
{
    OutputStream stream;
    uint16 iLength;
    if (!stream.Serialize(iAvatarId)) return -1;
    iLength = (uint16)strlen(strMessage);
    if (!stream.Serialize(iLength)) return -1;
    if (!stream.Serialize(iLength, strMessage)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_REGION<<8) | 4, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
