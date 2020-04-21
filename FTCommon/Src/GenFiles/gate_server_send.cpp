#include "gate_server_send.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

int32 GateServerSend::AvatarListAck(void* pServer, int32 iReturn, uint8 iAvatarCount, const ftdAvatar* arrayAvatar)
{
    OutputStream stream;
    uint16 arrayAvatar_length;
    if (!stream.Serialize(iReturn)) return -1;
    if (!stream.Serialize(iAvatarCount)) return -1;
    arrayAvatar_length = iAvatarCount;
    if (!stream.Serialize(arrayAvatar_length)) return -1;
    if (!stream.Serialize(arrayAvatar_length, arrayAvatar)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_GATE<<8) | 0, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 GateServerSend::AvatarCreateAck(void* pServer, int32 iReturn, const ftdAvatar& newAvatar)
{
    OutputStream stream;
    if (!stream.Serialize(iReturn)) return -1;
    if (!stream.Serialize(newAvatar)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_GATE<<8) | 1, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 GateServerSend::AvatarSelectAck(void* pServer, int32 iReturn, const ftdAvatarSelectData& data)
{
    OutputStream stream;
    if (!stream.Serialize(iReturn)) return -1;
    if (!stream.Serialize(data)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_GATE<<8) | 2, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 GateServerSend::ChannelListNtf(void* pServer, uint8 iChannelCount, const ftdChannelData* arrayChannelData)
{
    OutputStream stream;
    uint16 arrayChannelData_length;
    if (!stream.Serialize(iChannelCount)) return -1;
    arrayChannelData_length = iChannelCount;
    if (!stream.Serialize(arrayChannelData_length)) return -1;
    if (!stream.Serialize(arrayChannelData_length, arrayChannelData)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_GATE<<8) | 3, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 GateServerSend::ChannelSelectAck(void* pServer, int32 iReturn)
{
    OutputStream stream;
    if (!stream.Serialize(iReturn)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_GATE<<8) | 4, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 GateServerSend::ChannelLeaveAck(void* pServer)
{
    OutputStream stream;
    Sender::SendPacket(pServer, (SERVER_FILTER_GATE<<8) | 5, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
