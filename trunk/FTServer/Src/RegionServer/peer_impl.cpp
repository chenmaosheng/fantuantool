#include "region_server.h"
#include "region_server_loop.h"
#include "region_logic_command.h"

#include "region_peer_recv.h"
#include "session_peer_recv.h"

#include "memory_object.h"

void RegionPeerRecv::RegionAllocReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint64 iAvatarId, uint16 iLen, const TCHAR* strAvatarName)
{
	LogicCommandOnRegionAllocReq* pCommand = FT_NEW(LogicCommandOnRegionAllocReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnRegionAllocReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	pCommand->m_iAvatarId = iAvatarId;
	wcscpy_s(pCommand->m_strAvatarName, _countof(pCommand->m_strAvatarName), strAvatarName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void RegionPeerRecv::RegionReleaseReq(PEER_CLIENT pPeerClient, uint32 iSessionId)
{
	LogicCommandOnRegionReleaseReq* pCommand = FT_NEW(LogicCommandOnRegionReleaseReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnRegionReleaseReq) failed"));
		return;
	}
	
	pCommand->m_iSessionId = iSessionId;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void RegionPeerRecv::RegionEnterReq(PEER_CLIENT pPeerClient, uint32 iSessionId)
{
	LogicCommandOnRegionEnterReq* pCommand = FT_NEW(LogicCommandOnRegionEnterReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnRegionEnterReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void RegionPeerRecv::RegionEnterAck(PEER_CLIENT pPeerClient, uint32 iSessionId, int32 iReturn)
{
	LogicCommandOnRegionEnterAck* pCommand = FT_NEW(LogicCommandOnRegionEnterAck);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnRegionEnterAck) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	pCommand->m_iReturn = iReturn;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void RegionPeerRecv::RegionLeaveReq(PEER_CLIENT pPeerClient, uint32 iSessionId)
{
	LogicCommandOnRegionLeaveReq* pCommand = FT_NEW(LogicCommandOnRegionLeaveReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnRegionEnterAck) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void SessionPeerRecv::PacketForward(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf)
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
	_ASSERT(false);
}

void SessionPeerRecv::Disconnect(PEER_CLIENT pPeerClient, uint32 iSessionid, uint8 iReason)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
	_ASSERT(false);
}

void SessionPeerRecv::SendData(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	LOG_ERR(LOG_SERVER, _T("Impossible to arrive here"));
	_ASSERT(false);
}