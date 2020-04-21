#include "login_client_recv.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

namespace ft_LoginClient_recv
{

bool CALLBACK VersionReq_Callback(void* pClient, InputStream& stream)
{
    uint32 iVersion;
    if (!stream.Serialize(iVersion)) return false;
    LoginClientRecv::VersionReq(pClient, iVersion);
    return true;
}
static DispatchFilter::Func func[] = 
{
    VersionReq_Callback,
    NULL
};

struct LoginClientPacketDispatch
{
    LoginClientPacketDispatch()
    {
        DispatchFilterArray::GetFilter(CLIENT_FILTER_LOGIN).m_pFunc = func;
        DispatchFilterArray::GetFilter(CLIENT_FILTER_LOGIN).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static ft_LoginClient_recv::LoginClientPacketDispatch _LoginClientPacketDispatch;

