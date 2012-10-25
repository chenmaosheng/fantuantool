#include "master_logic_command.h"
#include "master_server.h"
#include "master_server_loop.h"

#include "master_peer_recv.h"
#include "session_peer_recv.h"
#include "login_peer_recv.h"

#include "memory_object.h"

void MasterPeerRecv::LoginReportState(PEER_CLIENT pPeerClient, uint8 iServerId)
{
	LogicCommandOnLoginReport* pCommand = FT_NEW(LogicCommandOnLoginReport);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnLoginReport) failed"));
		return;
	}

	pCommand->m_iServerId = iServerId;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::GateReportState(PEER_CLIENT pPeerClient, uint8 iServerId, uint16 iSessionCount)
{
	LogicCommandOnGateReport* pCommand = FT_NEW(LogicCommandOnGateReport);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnGateReport) failed"));
		return;
	}

	pCommand->m_iServerId = iServerId;
	pCommand->m_iSessionCount = iSessionCount;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::CacheReportState(PEER_CLIENT pPeerClient, uint8 iServerId)
{
	LogicCommandOnCacheReport* pCommand = FT_NEW(LogicCommandOnCacheReport);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnCacheReport) failed"));
		return;
	}

	pCommand->m_iServerId = iServerId;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::RegionReportState(PEER_CLIENT pPeerClient, uint8 iServerId, uint16 iPlayerCount)
{
	LogicCommandOnRegionReport* pCommand = FT_NEW(LogicCommandOnRegionReport);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnRegionReport) failed"));
		return;
	}

	pCommand->m_iServerId = iServerId;
	pCommand->m_iPlayerCount = iPlayerCount;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::OnLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
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

void MasterPeerRecv::GateAllocAck(PEER_CLIENT pPeerClient, uint8 iServerId, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR *strAccountName, uint32 iGateSessionId)
{
	LogicCommandGateAllocAck* pCommand = FT_NEW(LogicCommandGateAllocAck);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandGateAllocAck) failed"));
		return;
	}

	pCommand->m_iServerId = iServerId;
	pCommand->m_iLoginSessionId = iLoginSessionId;
	pCommand->m_iGateSessionId = iGateSessionId;
	wcscpy_s(pCommand->m_strAccountName, _countof(pCommand->m_strAccountName), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::OnGateLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
	LogicCommandOnGateLoginReq* pCommand = FT_NEW(LogicCommandOnGateLoginReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnGateLoginReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	wcscpy_s(pCommand->m_strAccountName, _countof(pCommand->m_strAccountName), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::OnRegionAllocAck(PEER_CLIENT pPeerClient, uint32 iSessionId, uint8 iServerId, int32 iReturn)
{
	LogicCommandOnRegionAllocAck* pCommand = FT_NEW(LogicCommandOnRegionAllocAck);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnRegionAllocAck) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	pCommand->m_iServerId = iServerId;
	pCommand->m_iReturn = iReturn;

	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::OnRegionLeaveReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint8 iServerId)
{
	LogicCommandOnRegionLeaveReq* pCommand = FT_NEW(LogicCommandOnRegionLeaveReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnRegionLeaveReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	pCommand->m_iRegionServerId = iServerId;

	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::OnRegionPlayerFailReq(PEER_CLIENT pPeerClient, uint32 iSessionId, int32 iReason)
{
	LogicCommandOnRegionPlayerFailReq* pCommand = FT_NEW(LogicCommandOnRegionPlayerFailReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnRegionPlayerFailReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	pCommand->m_iReason = iReason;

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
















void SessionPeerRecv::Disconnect(PEER_CLIENT pPeerClient, uint32 iSessionid, uint8 iReason)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}

void SessionPeerRecv::SendData(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}

void LoginPeerRecv::OnLoginFailedAck(PEER_CLIENT pPeerClient, uint32 iSessionId, int8 iReason)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
}
