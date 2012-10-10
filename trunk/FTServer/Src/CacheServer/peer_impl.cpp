#include "cache_logic_command.h"
#include "cache_server.h"
#include "cache_server_loop.h"

#include "cache_peer_recv.h"
#include "session_peer_recv.h"

#include "memory_object.h"

void CachePeerRecv::OnLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR *strAccountName)
{
	LogicCommandOnLoginReq* pCommand = FT_NEW(LogicCommandOnLoginReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnLoginReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	wcscpy_s(pCommand->m_strAccountName, _countof(pCommand->m_strAccountName), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void CachePeerRecv::OnLogoutReq(PEER_CLIENT pPeerClient, uint32 iSessionId)
{
	LogicCommandOnLogoutReq* pCommand = FT_NEW(LogicCommandOnLogoutReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnLogoutReq) failed"));
		return;
	}
	pCommand->m_iSessionId = iSessionId;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void SessionPeerRecv::PacketForward(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	LogicCommandPacketForward* pCommand = FT_NEW(LogicCommandPacketForward);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandPacketForward) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	pCommand->m_iTypeId = iTypeId;
	if (!pCommand->CopyData(iLen, pBuf))
	{
		LOG_ERR(LOG_SERVER, _T("Copy data failed"));
		FT_DELETE(pCommand);
		return;
	}

	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void SessionPeerRecv::OnSessionDisconnect(PEER_CLIENT pPeerClient, uint32 iSessionId)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}

void SessionPeerRecv::Disconnect(PEER_CLIENT pPeerClient, uint32 iSessionid, uint8 iReason)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}

void SessionPeerRecv::SendData(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}