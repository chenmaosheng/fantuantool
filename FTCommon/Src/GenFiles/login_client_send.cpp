#include "login_client_send.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

int32 LoginClientSend::VersionReq(void* pServer, uint32 iVersion)
{
    OutputStream stream;
    if (!stream.Serialize(iVersion)) return -1;
    Sender::SendPacket(pServer, (CLIENT_FILTER_LOGIN<<8) | 0, stream.GetDataLength(), stream.GetBuffer());
    return 0;
}
