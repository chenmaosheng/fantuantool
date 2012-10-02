#include "gate_peer_recv.h"
#include "gate_logic_command.h"
#include "gate_server.h"
#include "gate_server_loop.h"
#include "memory_object.h"
#include "session_peer_recv.h"

void GatePeerRecv::GateHoldReq(PEER_CLIENT pPeerClient, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
	uint32 iRet = 0;
	LogicCommandGateHoldReq* pCommand = FT_NEW(LogicCommandGateHoldReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandGateHoldReq) failed"));
		return;
	}

	pCommand->m_iLoginSessionId = iLoginSessionId;
	wcscpy_s(pCommand->m_strAccountName, _countof(pCommand->m_strAccountName), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void GatePeerRecv::GateReleaseReq(PEER_CLIENT pPeerClient, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
	uint32 iRet = 0;
	LogicCommandGateReleaseReq* pCommand = FT_NEW(LogicCommandGateReleaseReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandGateReleaseReq) failed"));
		return;
	}

	pCommand->m_iLoginSessionId = iLoginSessionId;
	wcscpy_s(pCommand->m_strAccountName, _countof(pCommand->m_strAccountName), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void SessionPeerRecv::Disconnect(PEER_CLIENT pPeerClient, uint32 iSessionId, uint8 iReason)
{
	LogicCommandDisconnect* pCommand = FT_NEW(LogicCommandDisconnect);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandGateReleaseReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	pCommand->m_iReason = iReason;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void SessionPeerRecv::OnSessionDisconnect(PEER_CLIENT pPeerClient, uint32 iSessionId)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}

void SessionPeerRecv::PacketForward(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}