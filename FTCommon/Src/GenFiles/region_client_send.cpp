#include "region_client_send.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

int32 RegionClientSend::ClientTimeReq(void* pServer, uint32 iClientTime)
{
    OutputStream stream;
    if (!stream.Serialize(iClientTime)) return -1;
    Sender::SendPacket(pServer, (CLIENT_FILTER_REGION<<8) | 0, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 RegionClientSend::RegionChatReq(void* pServer, const char* strMessage)
{
    OutputStream stream;
    uint16 iLength;
    iLength = (uint16)strlen(strMessage);
    if (!stream.Serialize(iLength)) return -1;
    if (!stream.Serialize(iLength, strMessage)) return -1;
    Sender::SendPacket(pServer, (CLIENT_FILTER_REGION<<8) | 1, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
