#include "gate_client_recv.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

namespace ft_GateClient_recv
{

bool CALLBACK AvatarListReq_Callback(void* pClient, InputStream& stream)
{
    GateClientRecv::AvatarListReq(pClient);
    return true;
}
bool CALLBACK AvatarCreateReq_Callback(void* pClient, InputStream& stream)
{
    ftdAvatarCreateData data;
    if (!stream.Serialize(data)) return false;
    GateClientRecv::AvatarCreateReq(pClient, data);
    return true;
}
bool CALLBACK AvatarSelectReq_Callback(void* pClient, InputStream& stream)
{
    char strAvatarName[MAX_OUTPUT_BUFFER] = {0};
    uint16 iLength;

    if (!stream.Serialize(iLength)) return false;
    if (!stream.Serialize(iLength, strAvatarName)) return false;
    strAvatarName[iLength] = '\0';
    GateClientRecv::AvatarSelectReq(pClient, strAvatarName);
    return true;
}
bool CALLBACK ChannelSelectReq_Callback(void* pClient, InputStream& stream)
{
    char strChannelName[MAX_OUTPUT_BUFFER] = {0};
    uint16 iLength;

    if (!stream.Serialize(iLength)) return false;
    if (!stream.Serialize(iLength, strChannelName)) return false;
    strChannelName[iLength] = '\0';
    GateClientRecv::ChannelSelectReq(pClient, strChannelName);
    return true;
}
bool CALLBACK ChannelLeaveReq_Callback(void* pClient, InputStream& stream)
{
    GateClientRecv::ChannelLeaveReq(pClient);
    return true;
}
static DispatchFilter::Func func[] = 
{
    AvatarListReq_Callback,
    AvatarCreateReq_Callback,
    AvatarSelectReq_Callback,
    ChannelSelectReq_Callback,
    ChannelLeaveReq_Callback,
    NULL
};

struct GateClientPacketDispatch
{
    GateClientPacketDispatch()
    {
        DispatchFilterArray::GetFilter(CLIENT_FILTER_GATE).m_pFunc = func;
        DispatchFilterArray::GetFilter(CLIENT_FILTER_GATE).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static ft_GateClient_recv::GateClientPacketDispatch _GateClientPacketDispatch;

