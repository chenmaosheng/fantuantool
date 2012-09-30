#include "master_peer_recv.h"
#include "master_logic_command.h"
#include "master_server.h"
#include "master_server_loop.h"
#include "memory_object.h"
#include "session_peer_recv.h"

void MasterPeerRecv::OnLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, const char* strAccountName)
{
	uint32 iRet = 0;
	LogicCommandOnLoginReq* pCommand = FT_NEW(LogicCommandOnLoginReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnLoginReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	iRet = Char2WChar(strAccountName, pCommand->m_strAccountName, sizeof(pCommand->m_strAccountName)/sizeof(TCHAR));
	if (iRet == 0)
	{
		LOG_ERR(LOG_SERVER, _T("Char2WChar failed"));
		FT_DELETE(pCommand);
		return;
	}

	pCommand->m_strAccountName[iRet] = _T('\0');
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::GateHoldAck(PEER_CLIENT pPeerClient, uint16 iServerId, uint32 iLoginSessionId, const TCHAR *strAccountName, uint32 iGateSessionId)
{
	LogicCommandGateHoldAck* pCommand = FT_NEW(LogicCommandGateHoldAck);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandGateHoldAck) failed"));
		return;
	}

	pCommand->m_iServerId = iServerId;
	pCommand->m_iLoginSessionId = iLoginSessionId;
	pCommand->m_iGateSessionId = iGateSessionId;
	wcscpy_s(pCommand->m_strAccountName, sizeof(pCommand->m_strAccountName)/sizeof(TCHAR), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void SessionPeerRecv::OnSessionDisconnect(PEER_CLIENT pPeerClient, uint32 iSessionId)
{
	LogicCommandOnSessionDisconnect* pCommand = FT_NEW(LogicCommandOnSessionDisconnect);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnSessionDisconnect) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	g_pServer->m_pMainLoop->PushCommand(pCommand);	
}

void SessionPeerRecv::PacketForward(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}