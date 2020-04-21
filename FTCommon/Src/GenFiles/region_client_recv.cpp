#include "region_client_recv.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

namespace ft_RegionClient_recv
{

bool CALLBACK ClientTimeReq_Callback(void* pClient, InputStream& stream)
{
    uint32 iClientTime;
    if (!stream.Serialize(iClientTime)) return false;
    RegionClientRecv::ClientTimeReq(pClient, iClientTime);
    return true;
}
bool CALLBACK RegionChatReq_Callback(void* pClient, InputStream& stream)
{
    char strMessage[MAX_OUTPUT_BUFFER] = {0};
    uint16 iLength;

    if (!stream.Serialize(iLength)) return false;
    if (!stream.Serialize(iLength, strMessage)) return false;
    strMessage[iLength] = '\0';
    RegionClientRecv::RegionChatReq(pClient, strMessage);
    return true;
}
static DispatchFilter::Func func[] = 
{
    ClientTimeReq_Callback,
    RegionChatReq_Callback,
    NULL
};

struct RegionClientPacketDispatch
{
    RegionClientPacketDispatch()
    {
        DispatchFilterArray::GetFilter(CLIENT_FILTER_REGION).m_pFunc = func;
        DispatchFilterArray::GetFilter(CLIENT_FILTER_REGION).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static ft_RegionClient_recv::RegionClientPacketDispatch _RegionClientPacketDispatch;

