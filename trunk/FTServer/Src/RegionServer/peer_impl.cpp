#include "region_peer_recv.h"
#include "region_server.h"
#include "region_server_loop.h"
#include "region_logic_command.h"

#include "memory_object.h"

void RegionPeerRecv::RegionAllocReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint64 iAvatarId, uint16 iLen, const TCHAR* strAvatarName)
{
	int32 iRet = 0;
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
}

void RegionPeerRecv::RegionEnterReq(PEER_CLIENT pPeerClient, uint32 iSessionId)
{
	int32 iRet = 0;
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
	int32 iRet = 0;
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