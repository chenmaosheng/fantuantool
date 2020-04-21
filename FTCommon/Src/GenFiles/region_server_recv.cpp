#include "region_server_recv.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

namespace ft_RegionServer_recv
{

bool CALLBACK ServerTimeNtf_Callback(void* pClient, InputStream& stream)
{
    uint32 iServerTime;
    if (!stream.Serialize(iServerTime)) return false;
    RegionServerRecv::ServerTimeNtf(pClient, iServerTime);
    return true;
}
bool CALLBACK InitialAvatarDataNtf_Callback(void* pClient, InputStream& stream)
{
    uint64 iAvatarId;
    char strAvatarName[MAX_OUTPUT_BUFFER] = {0};
    uint16 iLength;

    if (!stream.Serialize(iAvatarId)) return false;
    if (!stream.Serialize(iLength)) return false;
    if (!stream.Serialize(iLength, strAvatarName)) return false;
    strAvatarName[iLength] = '\0';
    RegionServerRecv::InitialAvatarDataNtf(pClient, iAvatarId, strAvatarName);
    return true;
}
bool CALLBACK RegionAvatarEnterNtf_Callback(void* pClient, InputStream& stream)
{
    uint64 iAvatarId;
    char strAvatarName[MAX_OUTPUT_BUFFER] = {0};
    uint16 iLength;

    if (!stream.Serialize(iAvatarId)) return false;
    if (!stream.Serialize(iLength)) return false;
    if (!stream.Serialize(iLength, strAvatarName)) return false;
    strAvatarName[iLength] = '\0';
    RegionServerRecv::RegionAvatarEnterNtf(pClient, iAvatarId, strAvatarName);
    return true;
}
bool CALLBACK RegionAvatarLeaveNtf_Callback(void* pClient, InputStream& stream)
{
    uint64 iAvatarId;
    if (!stream.Serialize(iAvatarId)) return false;
    RegionServerRecv::RegionAvatarLeaveNtf(pClient, iAvatarId);
    return true;
}
bool CALLBACK RegionChatNtf_Callback(void* pClient, InputStream& stream)
{
    uint64 iAvatarId;
    char strMessage[MAX_OUTPUT_BUFFER] = {0};
    uint16 iLength;

    if (!stream.Serialize(iAvatarId)) return false;
    if (!stream.Serialize(iLength)) return false;
    if (!stream.Serialize(iLength, strMessage)) return false;
    strMessage[iLength] = '\0';
    RegionServerRecv::RegionChatNtf(pClient, iAvatarId, strMessage);
    return true;
}
static DispatchFilter::Func func[] = 
{
    ServerTimeNtf_Callback,
    InitialAvatarDataNtf_Callback,
    RegionAvatarEnterNtf_Callback,
    RegionAvatarLeaveNtf_Callback,
    RegionChatNtf_Callback,
    NULL
};

struct RegionServerPacketDispatch
{
    RegionServerPacketDispatch()
    {
        DispatchFilterArray::GetFilter(SERVER_FILTER_REGION).m_pFunc = func;
        DispatchFilterArray::GetFilter(SERVER_FILTER_REGION).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static ft_RegionServer_recv::RegionServerPacketDispatch _RegionServerPacketDispatch;

