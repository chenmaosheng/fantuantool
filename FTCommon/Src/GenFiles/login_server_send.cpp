#include "login_server_send.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

int32 LoginServerSend::VersionAck(void* pServer, int32 iReason)
{
    OutputStream stream;
    if (!stream.Serialize(iReason)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_LOGIN<<8) | 0, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 LoginServerSend::LoginFailedAck(void* pServer, int32 iReason)
{
    OutputStream stream;
    if (!stream.Serialize(iReason)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_LOGIN<<8) | 1, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
int32 LoginServerSend::LoginNtf(void* pServer, uint32 iGateIP, uint16 iGatePort)
{
    OutputStream stream;
    if (!stream.Serialize(iGateIP)) return -1;
    if (!stream.Serialize(iGatePort)) return -1;
    Sender::SendPacket(pServer, (SERVER_FILTER_LOGIN<<8) | 2, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
