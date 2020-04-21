#include "login_peer_recv.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

namespace pr_LoginPeer_recv
{

bool CALLBACK OnLoginFailedAck_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    int8 iReason;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iReason)) return false;
    LoginPeerRecv::OnLoginFailedAck(pPeerClient, iSessionId, iReason);
    return true;
}

static PeerClientDispatchFilter::Func func[] = 
{
    OnLoginFailedAck_Callback,
    NULL
};

struct LoginPeerDispatch
{
    LoginPeerDispatch()
    {
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_LOGIN).m_pFunc = func;
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_LOGIN).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static pr_LoginPeer_recv::LoginPeerDispatch _LoginPeerDispatch;

