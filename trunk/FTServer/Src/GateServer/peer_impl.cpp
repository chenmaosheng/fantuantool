#include "gate_peer_recv.h"
#include "gate_logic_command.h"
#include "gate_server.h"
#include "gate_server_loop.h"
#include "memory_object.h"
#include "session_peer_recv.h"

void GatePeerRecv::GateAllocReq(PEER_CLIENT pPeerClient, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
	LogicCommandGateAllocReq* pCommand = FT_NEW(LogicCommandGateAllocReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandGateAllocReq) failed"));
		return;
	}

	pCommand->m_iLoginSessionId = iLoginSessionId;
	wcscpy_s(pCommand->m_strAccountName, _countof(pCommand->m_strAccountName), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void GatePeerRecv::GateReleaseReq(PEER_CLIENT pPeerClient, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
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

void GatePeerRecv::RegionBindReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint8 iRegionServerId)
{
	LogicCommandRegionBindReq* pCommand = FT_NEW(LogicCommandRegionBindReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandRegionBindReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	pCommand->m_iRegionServerId = iRegionServerId;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void GatePeerRecv::BroadcastData(PEER_CLIENT pPeerClient, uint16 iSessionCount, const uint32* arraySessionId, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	LogicCommandBroadcastData* pCommand = FT_NEW(LogicCommandBroadcastData);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandBroadcastData) failed"));
		return;
	}

	pCommand->m_iSessionCount = iSessionCount;
	for (uint16 i = 0; i < iSessionCount; ++i)
	{
		pCommand->m_arraySessionId[i] = arraySessionId[i];
	}
	pCommand->m_iTypeId = iTypeId;
	if (!pCommand->CopyData(iLen, pBuf))
	{
		LOG_ERR(LOG_SERVER, _T("Copy data failed"));
		FT_DELETE(pCommand);
		return;
	}

	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void SessionPeerRecv::Disconnect(PEER_CLIENT pPeerClient, uint32 iSessionId, uint8 iReason)
{
	LogicCommandDisconnect* pCommand = FT_NEW(LogicCommandDisconnect);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandDisconnect) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	pCommand->m_iReason = iReason;
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

void SessionPeerRecv::SendData(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	LogicCommandSendData* pCommand = FT_NEW(LogicCommandSendData);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandSendData) failed"));
		return;
	}

	LOG_DBG(LOG_SERVER, _T("sid=%d iTypeId=%d iLen=%d"), iSessionId, iTypeId, iLen);

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
	_ASSERT(false);
}