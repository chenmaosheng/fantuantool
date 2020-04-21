#include "login_server_recv.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

namespace ft_LoginServer_recv
{

bool CALLBACK VersionAck_Callback(void* pClient, InputStream& stream)
{
    int32 iReason;
    if (!stream.Serialize(iReason)) return false;
    LoginServerRecv::VersionAck(pClient, iReason);
    return true;
}
bool CALLBACK LoginFailedAck_Callback(void* pClient, InputStream& stream)
{
    int32 iReason;
    if (!stream.Serialize(iReason)) return false;
    LoginServerRecv::LoginFailedAck(pClient, iReason);
    return true;
}
bool CALLBACK LoginNtf_Callback(void* pClient, InputStream& stream)
{
    uint32 iGateIP;
    uint16 iGatePort;
    if (!stream.Serialize(iGateIP)) return false;
    if (!stream.Serialize(iGatePort)) return false;
    LoginServerRecv::LoginNtf(pClient, iGateIP, iGatePort);
    return true;
}
static DispatchFilter::Func func[] = 
{
    VersionAck_Callback,
    LoginFailedAck_Callback,
    LoginNtf_Callback,
    NULL
};

struct LoginServerPacketDispatch
{
    LoginServerPacketDispatch()
    {
        DispatchFilterArray::GetFilter(SERVER_FILTER_LOGIN).m_pFunc = func;
        DispatchFilterArray::GetFilter(SERVER_FILTER_LOGIN).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static ft_LoginServer_recv::LoginServerPacketDispatch _LoginServerPacketDispatch;

