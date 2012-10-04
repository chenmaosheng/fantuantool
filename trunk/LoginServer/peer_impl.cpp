#include "login_session.h"
#include "session_peer_recv.h"
#include "login_logic_command.h"
#include "login_server.h"
#include "login_server_loop.h"
#include "login_peer_recv.h"

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

void LoginPeerRecv::OnLoginFailedAck(PEER_CLIENT pPeerClient, uint32 iSessionId, int8 iReason)
{
	LogicCommandOnLoginFailedAck* pCommand = FT_NEW(LogicCommandOnLoginFailedAck);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnLoginFailedAck) failed"));
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

void SessionPeerRecv::Disconnect(PEER_CLIENT pPeerClient, uint32 iSessionid, uint8 iReason)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}

