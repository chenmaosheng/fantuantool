#include "gate_server_recv.h"
#include "packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

namespace ft_GateServer_recv
{

bool CALLBACK AvatarListAck_Callback(void* pClient, InputStream& stream)
{
    int32 iReturn;
    uint8 iAvatarCount;
    ftdAvatar* arrayAvatar;
    uint16 arrayAvatar_length;
    if (!stream.Serialize(iReturn)) return false;
    if (!stream.Serialize(iAvatarCount)) return false;
    arrayAvatar_length = iAvatarCount;
    if (!stream.Serialize(arrayAvatar_length)) return false;
    arrayAvatar = (ftdAvatar*)_malloca(sizeof(ftdAvatar)*arrayAvatar_length);
    if (!stream.Serialize(arrayAvatar_length, arrayAvatar)) return false;
    GateServerRecv::AvatarListAck(pClient, iReturn, iAvatarCount, arrayAvatar);
    return true;
}
bool CALLBACK AvatarCreateAck_Callback(void* pClient, InputStream& stream)
{
    int32 iReturn;
    ftdAvatar newAvatar;
    if (!stream.Serialize(iReturn)) return false;
    if (!stream.Serialize(newAvatar)) return false;
    GateServerRecv::AvatarCreateAck(pClient, iReturn, newAvatar);
    return true;
}
bool CALLBACK AvatarSelectAck_Callback(void* pClient, InputStream& stream)
{
    int32 iReturn;
    ftdAvatarSelectData data;
    if (!stream.Serialize(iReturn)) return false;
    if (!stream.Serialize(data)) return false;
    GateServerRecv::AvatarSelectAck(pClient, iReturn, data);
    return true;
}
bool CALLBACK ChannelListNtf_Callback(void* pClient, InputStream& stream)
{
    uint8 iChannelCount;
    ftdChannelData* arrayChannelData;
    uint16 arrayChannelData_length;
    if (!stream.Serialize(iChannelCount)) return false;
    arrayChannelData_length = iChannelCount;
    if (!stream.Serialize(arrayChannelData_length)) return false;
    arrayChannelData = (ftdChannelData*)_malloca(sizeof(ftdChannelData)*arrayChannelData_length);
    if (!stream.Serialize(arrayChannelData_length, arrayChannelData)) return false;
    GateServerRecv::ChannelListNtf(pClient, iChannelCount, arrayChannelData);
    return true;
}
bool CALLBACK ChannelSelectAck_Callback(void* pClient, InputStream& stream)
{
    int32 iReturn;
    if (!stream.Serialize(iReturn)) return false;
    GateServerRecv::ChannelSelectAck(pClient, iReturn);
    return true;
}
bool CALLBACK ChannelLeaveAck_Callback(void* pClient, InputStream& stream)
{
    GateServerRecv::ChannelLeaveAck(pClient);
    return true;
}
static DispatchFilter::Func func[] = 
{
    AvatarListAck_Callback,
    AvatarCreateAck_Callback,
    AvatarSelectAck_Callback,
    ChannelListNtf_Callback,
    ChannelSelectAck_Callback,
    ChannelLeaveAck_Callback,
    NULL
};

struct GateServerPacketDispatch
{
    GateServerPacketDispatch()
    {
        DispatchFilterArray::GetFilter(SERVER_FILTER_GATE).m_pFunc = func;
        DispatchFilterArray::GetFilter(SERVER_FILTER_GATE).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static ft_GateServer_recv::GateServerPacketDispatch _GateServerPacketDispatch;

