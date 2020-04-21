#include "gate_client_send.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

int32 GateClientSend::AvatarListReq(void* pServer)
{
    OutputStream stream;
    Sender::SendPacket(pServer, (CLIENT_FILTER_GATE<<8) | 0, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 GateClientSend::AvatarCreateReq(void* pServer, const ftdAvatarCreateData& data)
{
    OutputStream stream;
    if (!stream.Serialize(data)) return -1;
    Sender::SendPacket(pServer, (CLIENT_FILTER_GATE<<8) | 1, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 GateClientSend::AvatarSelectReq(void* pServer, const char* strAvatarName)
{
    OutputStream stream;
    uint16 iLength;
    iLength = (uint16)strlen(strAvatarName);
    if (!stream.Serialize(iLength)) return -1;
    if (!stream.Serialize(iLength, strAvatarName)) return -1;
    Sender::SendPacket(pServer, (CLIENT_FILTER_GATE<<8) | 2, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 GateClientSend::ChannelSelectReq(void* pServer, const char* strChannelName)
{
    OutputStream stream;
    uint16 iLength;
    iLength = (uint16)strlen(strChannelName);
    if (!stream.Serialize(iLength)) return -1;
    if (!stream.Serialize(iLength, strChannelName)) return -1;
    Sender::SendPacket(pServer, (CLIENT_FILTER_GATE<<8) | 3, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 GateClientSend::ChannelLeaveReq(void* pServer)
{
    OutputStream stream;
    Sender::SendPacket(pServer, (CLIENT_FILTER_GATE<<8) | 4, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
