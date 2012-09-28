#include "master_player_context.h"
#include "master_server_loop.h"
#include "gate_peer_send.h"
#include "master_server.h"
#include "master_server_config.h"
#include "login_server_send.h"
#include "session_peer_send.h"
#include "packet.h"

MasterServerLoop* MasterPlayerContext::m_pMainLoop = NULL;
uint16 MasterPlayerContext::m_iDelayTypeId = 0;
uint16 MasterPlayerContext::m_iDelayLen = 0;
char MasterPlayerContext::m_DelayBuf[MAX_INPUT_BUFFER] = {0};

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

int32 MasterPlayerContext::DelaySendData(uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	m_iDelayTypeId = iTypeId;
	m_iDelayLen = iLen;
	memcpy(m_DelayBuf, pBuf, iLen);

	return 0;
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

void MasterPlayerContext::GateHoldAck(uint16 iGateServerId, uint32 iGateSessionId)
{
	int32 iRet = 0;
	GateConfigItem* pConfigItem = NULL;

	m_iGateServerId = iGateServerId;
	pConfigItem = g_pServerConfig->GetGateConfigItem(iGateServerId);
	if (!pConfigItem)
	{
		return;
	}

	// send gate server's ip and port to client
	iRet = LoginServerSend::LoginNtf(this, pConfigItem->m_iServerIP, pConfigItem->m_iServerPort);
	if (iRet != 0)
	{
		return;
	}

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pLoginServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		return;
	}

	// todo: login session id delete, add gate session id
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((MasterPlayerContext*)pClient)->DelaySendData(iTypeId, iLen, pBuf);
}

