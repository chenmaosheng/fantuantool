#include "gate_session.h"
#include "gate_server_loop.h"
#include "gate_server.h"
#include "master_peer_send.h"
#include "gate_server_config.h"
#include "packet.h"

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
	LOG_DBG(LOG_SERVER, _T("Receive a connection, connId=%d"), connId);
	return super::OnConnection(connId);
}

void GateSession::OnDisconnect()
{
	LOG_DBG(LOG_SERVER, _T("sid=%d connection is disconnected"), m_iSessionId);
	super::OnDisconnect();
}

void GateSession::Disconnect()
{
	LOG_DBG(LOG_SERVER, _T("sid=%d force to disconnect"), m_iSessionId);
	super::Disconnect();
}

void GateSession::OnHoldReq(uint32 iLoginSessionId, const TCHAR *strAccountName)
{
	int32 iRet = 0;

	((SessionId*)&m_iSessionId)->sValue_.sequence_++;
	wcscpy_s(m_strAccountName, sizeof(m_strAccountName)/sizeof(TCHAR), strAccountName);

	iRet = MasterPeerSend::GateHoldAck(g_pServer->m_pMasterServer, g_pServerConfig->m_iServerId, iLoginSessionId, strAccountName, m_iSessionId);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%d gate hold ack failed to send"), m_iSessionId);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("sid=%d send gate hold ack to master server"), m_iSessionId);
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((GateSession*)pClient)->SendData(iTypeId, iLen, pBuf);
}
