#include "master_player_context.h"
#include "master_server_loop.h"
#include "gate_peer_send.h"
#include "master_server.h"

MasterServerLoop* MasterPlayerContext::m_pMainLoop = NULL;

MasterPlayerContext::MasterPlayerContext()
{
	Clear();
}

MasterPlayerContext::~MasterPlayerContext()
{
	Clear();
}

void MasterPlayerContext::Clear()
{
	m_iSessionId = 0;
	m_strAccountName[0] = '\0';
	m_iGateServerId = 0;
}

void MasterPlayerContext::OnLoginReq(uint32 iSessionId, const TCHAR* strAccountName)
{
	int32 iRet = 0;
	
	m_iSessionId = iSessionId;
	wcscpy_s(m_strAccountName, sizeof(m_strAccountName)/sizeof(TCHAR), strAccountName);

	// notify gate
	iRet = m_pMainLoop->GateHoldReq();
	if (iRet < 0)
	{
		return;
	}

	m_iGateServerId = (uint16)iRet;

	iRet = GatePeerSend::GateHoldReq(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, strAccountName);
	if (iRet != 0)
	{
		return;
	}
}
