#include "gate_session.h"
#include "gate_server_loop.h"
#include "gate_server.h"
#include "master_peer_send.h"

GateServerLoop* GateSession::m_pMainLoop = NULL;

GateSession::GateSession()
{
	Clear();
}

GateSession::~GateSession()
{
	Clear();
}

void GateSession::Clear()
{
	super::Clear();
}

int32 GateSession::OnConnection(ConnID connId)
{
	return super::OnConnection(connId);
}

void GateSession::OnDisconnect()
{
	super::OnDisconnect();
}

void GateSession::Disconnect()
{
	super::Disconnect();
}

void GateSession::OnHoldReq(uint32 iLoginSessionId, const TCHAR *strAccountName)
{
	int32 iRet = 0;

	((SessionId*)&m_iSessionId)->sValue_.sequence_++;
	wcscpy_s(m_strAccountName, sizeof(m_strAccountName)/sizeof(TCHAR), strAccountName);

	iRet = MasterPeerSend::GateHoldAck(g_pServer->m_pMasterServer, g_pConfig->m_iServerId, iLoginSessionId, strAccountName, m_iSessionId);
	if (iRet != 0)
	{
		return;
	}
}

