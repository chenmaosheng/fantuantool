#include "region_server_loop.h"
#include "region_player_context.h"
#include "region_server_config.h"
#include "region_server.h"
#include "region_logic_command.h"
#include "map.h"
#include "region_logic_loop.h"

#include "session.h"
#include "data_center.h"
#include "map_desc.h"
#include "auto_locker.h"

GateServerContext::GateServerContext()
{
	m_arrayPlayerContext = new RegionPlayerContext*[g_pServerConfig->GetGateSessionIndexMax()];
	memset(m_arrayPlayerContext, 0, sizeof(RegionPlayerContext*)*g_pServerConfig->GetGateSessionIndexMax());
}

GateServerContext::~GateServerContext()
{
	SAFE_DELETE_ARRAY(m_arrayPlayerContext);
}

RegionServerLoop::RegionServerLoop() :
m_iShutdownStatus(NOT_SHUTDOWN),
// need give a bit tolerance
m_PlayerContextPool(g_pServerConfig->m_iPlayerMax + g_pServerConfig->m_iPlayerMax / 10)
{
	memset(m_arrayGateServerContext, 0, sizeof(m_arrayGateServerContext));
	m_iPlayerCount = 0;
	m_iPlayerMax = g_pServerConfig->m_iPlayerMax;
	memset(m_arrayLogicLoop, 0, sizeof(m_arrayLogicLoop));
	m_iLogicLoopCount = 0;
}

int32 RegionServerLoop::Init(bool bMainLoop)
{
	int32 iRet = 0;
	const std::vector<MapDesc*>& vMapDesc = g_pDataCenter->GetAllMapDesc();
	
	iRet = super::Init();
	if (iRet != 0)
	{
		return iRet;
	}

	RegionPlayerContext::m_pMainLoop = this;
	m_iLogicLoopCount = g_pServerConfig->m_vRegionDesc.size();
	for (uint32 i = 0; i < m_iLogicLoopCount; ++i)
	{
		m_arrayLogicLoop[i] = new RegionLogicLoop(i);
		iRet = m_arrayLogicLoop[i]->Init(g_pServerConfig->m_vRegionDesc.at(i).m_arrayMapList, g_pServerConfig->m_vRegionDesc.at(i).m_iInstanceCount);
		if (iRet != 0)
		{
			return iRet;
		}
		LOG_DBG(LOG_SERVER, _T("Initialize logic loop%d success"), i);
	}

	// check each map
	for (uint32 i = 0; i < vMapDesc.size(); ++i)
	{
		MapDesc* pMapDesc = vMapDesc.at(i);
		if (!GetLogicLoopByMap(pMapDesc->m_iMapId))
		{
			LOG_ERR(LOG_SERVER, _T("Map is not included in logic loop, id=%d"), pMapDesc->m_iMapId);
			return -1;
		}
	}

	// initialize broadcast helper
	for(uint32 i = 0; i < SERVERCOUNT_MAX; ++i)
	{
		m_BroadcastHelper.SetGateServer(i, g_pServer->GetPeerServer(i));
	}
	
	return 0;
}

void RegionServerLoop::Destroy()
{
	for (uint32 i = 0; i < m_iLogicLoopCount; ++i)
	{
		m_arrayLogicLoop[i]->Destroy();
	}

	for (uint32 i = 0; i < SERVERCOUNT_MAX; ++i)
	{
		if (m_arrayGateServerContext[i])
		{
			SAFE_DELETE(m_arrayGateServerContext[i]);
		}
	}

	super::Destroy();
}

int32 RegionServerLoop::Start()
{
	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	for (uint32 i = 0; i < m_iLogicLoopCount; ++i)
	{
		m_arrayLogicLoop[i]->Start();
		LOG_DBG(LOG_SERVER, _T("Logic loop%d started"), i)
	}

	return 0;
}

bool RegionServerLoop::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

void RegionServerLoop::ShutdownPlayer(RegionPlayerContext* pPlayerContext)
{
	int32 iRet = 0;
	bool bNeedRegionLeave = false;
	bool bNeedDelete = false;

	if (pPlayerContext->m_bFinalizing)
	{
		return;
	}

	switch(pPlayerContext->m_StateMachine.GetCurrState())
	{
	case PLAYER_STATE_ONREGIONALLOCREQ:
	case PLAYER_STATE_REGIONALLOCACK:
	case PLAYER_STATE_ONREGIONRELEASEREQ:
	case PLAYER_STATE_ONREGIONENTERREQ:
	case PLAYER_STATE_REGIONENTERREQ:
	case PLAYER_STATE_ONREGIONENTERACK:
	case PLAYER_STATE_ONREGIONLEAVEREQ:
	case PLAYER_STATE_SERVERTIMENTF:
	case PLAYER_STATE_ONCLIENTTIMEREQ:
	case PLAYER_STATE_SERVERTIME2NTF:
	case PLAYER_STATE_INITAVATARNTF:
		bNeedRegionLeave = true;
		bNeedDelete = true;
		break;
	}

	if (bNeedRegionLeave)
	{

	}

	if (bNeedDelete)
	{
		AddPlayerToFinalizingQueue(pPlayerContext);
	}
}

void RegionServerLoop::AddPlayerToFinalizingQueue(RegionPlayerContext* pPlayerContext)
{
	pPlayerContext->m_bFinalizing = true;
	m_PlayerFinalizingQueue.push(pPlayerContext);
}

void RegionServerLoop::DeletePlayer(RegionPlayerContext* pPlayerContext)
{
	uint8 iServerId = 0;
	uint16 iSessionIndex = 0;
	uint32 iSessionId = pPlayerContext->m_iSessionId;

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x DeletePlayer"), pPlayerContext->m_strAvatarName, pPlayerContext->m_iAvatarId, iSessionId);

	stdext::hash_map<uint64, RegionPlayerContext*>::iterator mit = m_mPlayerContextByAvatarId.find(pPlayerContext->m_iAvatarId);
	if (mit != m_mPlayerContextByAvatarId.end())
	{
		// can't guarantee two avatars with same charid have same sessionid
		if (mit->second->m_iSessionId == iSessionId)
		{
			m_mPlayerContextByAvatarId.erase(mit);
		}
	}

	iServerId = ((SessionId*)&iSessionId)->sValue_.serverId_;
	iSessionIndex = ((SessionId*)&iSessionId)->sValue_.session_index_;
	m_arrayGateServerContext[iServerId]->m_arrayPlayerContext[iSessionIndex] = NULL;

	pPlayerContext->Clear();
	m_iPlayerCount--;
	m_PlayerContextPool.Free(pPlayerContext);
}

void RegionServerLoop::PushShutdownPlayerCommand(RegionPlayerContext* pPlayerContext)
{
	LogicCommandShutdownPlayerReq* pCommand = FT_NEW(LogicCommandShutdownPlayerReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandShutdownPlayerReq) failed"));
		return;
	}

	pCommand->m_pPlayerContext = pPlayerContext;
	PushCommand(pCommand);
}

RegionPlayerContext* RegionServerLoop::GetPlayerContextBySessionId(uint32 iSessionId)
{
	uint8 iServerId = ((SessionId*)&iSessionId)->sValue_.serverId_;
	uint16 iSessionIndex = ((SessionId*)&iSessionId)->sValue_.session_index_;

	if (iServerId >= SERVERCOUNT_MAX ||
		iSessionIndex > g_pServerConfig->GetGateSessionIndexMax())
	{
		return NULL;
	}

	GateServerContext* pContext = m_arrayGateServerContext[iServerId];
	if (!pContext)
	{
		return NULL;
	}

	return pContext->m_arrayPlayerContext[iSessionIndex];
}

RegionPlayerContext* RegionServerLoop::GetPlayerContextByAvatarId(uint64 iAvatarId)
{
	stdext::hash_map<uint64, RegionPlayerContext*>::iterator mit = m_mPlayerContextByAvatarId.find(iAvatarId);
	if (mit != m_mPlayerContextByAvatarId.end())
	{
		return mit->second;
	}

	return NULL;
}

RegionLogicLoop* RegionServerLoop::GetLogicLoopByMap(uint16 iMapId)
{
	stdext::hash_map<uint16, Map*>::iterator mit = m_mMapById.find(iMapId);
	if (mit != m_mMapById.end())
	{
		return mit->second->m_pRegionLogicLoop;
	}

	return NULL;
}

void RegionServerLoop::BindNewMap(uint16 iMapId, Map* pMap)
{
	m_mMapById.insert(std::make_pair(iMapId, pMap));
}

void RegionServerLoop::ReleaseMap(uint16 iMapId)
{
	stdext::hash_map<uint16, Map*>::iterator mit = m_mMapById.find(iMapId);
	if (mit != m_mMapById.end())
	{
		m_mMapById.erase(mit);
	}
}

void RegionServerLoop::BroadcastData(uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	m_BroadcastHelper.Clear();
	for (stdext::hash_map<uint64, RegionPlayerContext*>::iterator mit = m_mPlayerContextByAvatarId.begin(); mit != m_mPlayerContextByAvatarId.end(); ++mit)
	{
		m_BroadcastHelper.AddGateSession(mit->second->m_iSessionId);
	}
	m_BroadcastHelper.SendData(iTypeId, iLen, pBuf);
}

void RegionServerLoop::SendRegionAvatars(RegionPlayerContext* pPlayerContext)
{
	for (stdext::hash_map<uint64, RegionPlayerContext*>::iterator mit = m_mPlayerContextByAvatarId.begin(); mit != m_mPlayerContextByAvatarId.end(); ++mit)
	{
		if (pPlayerContext != mit->second)
		{
			pPlayerContext->SendAvatarEnterNtf(mit->second);
		}
	}
}

DWORD RegionServerLoop::_Loop()
{
	while (!m_PlayerFinalizingQueue.empty())
	{
		DeletePlayer(m_PlayerFinalizingQueue.back());
		m_PlayerFinalizingQueue.pop();
	}

	return 10;
}

bool RegionServerLoop::_OnCommand(LogicCommand* pCommand)
{
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_SHUTDOWN:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandShutdown();
		}
		break;

	case COMMAND_ONREGIONALLOCREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandOnRegionAllocReq((LogicCommandOnRegionAllocReq*)pCommand);
		}
		break;

	case COMMAND_ONREGIONRELEASEREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandOnRegionReleaseReq((LogicCommandOnRegionReleaseReq*)pCommand);
		}
		break;

	case COMMAND_ONREGIONENTERREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandOnRegionEnterReq((LogicCommandOnRegionEnterReq*)pCommand);
		}
		break;

	case COMMAND_ONREGIONENTERACK:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandOnRegionEnterAck((LogicCommandOnRegionEnterAck*)pCommand);
		}
		break;

	case COMMAND_ONREGIONLEAVEREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandOnRegionLeaveReq((LogicCommandOnRegionLeaveReq*)pCommand);
		}
		break;

	case COMMAND_PACKETFORWARD:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandPacketForward((LogicCommandPacketForward*)pCommand);
		}
		break;

	case COMMAND_SHUTDOWNPLAYERREQ:
		_OnCommandShutdownPlayerReq((LogicCommandShutdownPlayerReq*)pCommand);
		break;

	default:
		break;
	}

	return true;
}

void RegionServerLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;
}

bool RegionServerLoop::_PushPlayerToGateServerContext(uint32 iSessionId, RegionPlayerContext* pPlayerContext)
{
	uint8 iServerId = ((SessionId*)&iSessionId)->sValue_.serverId_;
	uint16 iSessionIndex = ((SessionId*)&iSessionId)->sValue_.session_index_;

	GateServerContext* pContext = m_arrayGateServerContext[iServerId];
	if (!pContext)
	{
		pContext = new GateServerContext;
		m_arrayGateServerContext[iServerId] = pContext;
	}

	// if session index is allocated
	if (pContext->m_arrayPlayerContext[iSessionIndex])
	{
		LOG_ERR(LOG_PLAYER, _T("sid=%08x session index is allocated"), iSessionId);
		_ASSERT(false);
		return false;
	}

	pContext->m_arrayPlayerContext[iSessionIndex] = pPlayerContext;
	return true;
}

void RegionServerLoop::_PopPlayerFromGateServerContext(uint32 iSessionId)
{
	uint8 iServerId = ((SessionId*)&iSessionId)->sValue_.serverId_;
	uint16 iSessionIndex = ((SessionId*)&iSessionId)->sValue_.session_index_;

	GateServerContext* pContext = m_arrayGateServerContext[iServerId];
	if (!pContext)
	{
		return;
	}

	pContext->m_arrayPlayerContext[iSessionIndex] = NULL;
}

void RegionServerLoop::_OnCommandOnRegionAllocReq(LogicCommandOnRegionAllocReq* pCommand)
{
	int32 iRet = 0;
	uint8 iServerId = 0;
	uint16 iSessionIndex = 0;
	GateServerContext* pContext = NULL;
	RegionPlayerContext* pPlayerContext = NULL;

	iServerId = ((SessionId*)&pCommand->m_iSessionId)->sValue_.serverId_;
	iSessionIndex = ((SessionId*)&pCommand->m_iSessionId)->sValue_.session_index_;

	// check session id is valid
	if (iServerId >= SERVERCOUNT_MAX || iSessionIndex >= g_pServerConfig->GetGateSessionIndexMax())
	{
		LOG_ERR(LOG_PLAYER, _T("sid=%08x serverId=%d or session index=%d invalid"), pCommand->m_iSessionId, iServerId, iSessionIndex);
		_ASSERT(false);
		return;
	}

	// allocate a new player
	pPlayerContext = m_PlayerContextPool.Allocate();
	if (!pPlayerContext)
	{
		LOG_ERR(LOG_PLAYER, _T("sid=%08x alloc failed"), pCommand->m_iSessionId);
		return;
	}

	LOG_DBG(LOG_PLAYER, _T("name=%s aid=%llu sid=%08x alloc a new player"), pCommand->m_strAvatarName, pCommand->m_iAvatarId, pCommand->m_iSessionId);

	m_iPlayerCount++;
	m_mPlayerContextByAvatarId.insert(std::make_pair(pCommand->m_iAvatarId, pPlayerContext));

	// put player into gate context
	_PushPlayerToGateServerContext(pCommand->m_iSessionId, pPlayerContext);

	pPlayerContext->OnRegionAllocReq(pCommand->m_iSessionId, pCommand->m_iAvatarId, pCommand->m_strAvatarName);
}

void RegionServerLoop::_OnCommandOnRegionReleaseReq(LogicCommandOnRegionReleaseReq* pCommand)
{
	RegionPlayerContext* pPlayerContext = GetPlayerContextBySessionId(pCommand->m_iSessionId);
	if (!pPlayerContext)
	{
		LOG_ERR(LOG_PLAYER, _T("sid=%08x can't find player"), pCommand->m_iSessionId);
		return;
	}

	pPlayerContext->OnRegionReleaseReq();
}

void RegionServerLoop::_OnCommandOnRegionEnterReq(LogicCommandOnRegionEnterReq* pCommand)
{
	RegionPlayerContext* pPlayerContext = GetPlayerContextBySessionId(pCommand->m_iSessionId);
	if (!pPlayerContext)
	{
		LOG_ERR(LOG_PLAYER, _T("sid=%08x can't find player"), pCommand->m_iSessionId);
		return;
	}

	pPlayerContext->OnRegionEnterReq();
}

void RegionServerLoop::_OnCommandOnRegionEnterAck(LogicCommandOnRegionEnterAck* pCommand)
{
	RegionPlayerContext* pPlayerContext = GetPlayerContextBySessionId(pCommand->m_iSessionId);
	if (!pPlayerContext)
	{
		LOG_ERR(LOG_PLAYER, _T("sid=%08x can't find player"), pCommand->m_iSessionId);
		return;
	}

	if (pCommand->m_iReturn != 0)
	{
		LOG_ERR(LOG_PLAYER, _T("sid=%08x region enter failed"), pCommand->m_iSessionId);
		ShutdownPlayer(pPlayerContext);
		return;
	}

	pPlayerContext->OnRegionEnterAck();
}

void RegionServerLoop::_OnCommandOnRegionLeaveReq(LogicCommandOnRegionLeaveReq* pCommand)
{
	RegionPlayerContext* pPlayerContext = GetPlayerContextBySessionId(pCommand->m_iSessionId);
	if (!pPlayerContext)
	{
		LOG_ERR(LOG_PLAYER, _T("sid=%08x can't find player"), pCommand->m_iSessionId);
		return;
	}

	pPlayerContext->OnRegionLeaveReq();
}

void RegionServerLoop::_OnCommandPacketForward(LogicCommandPacketForward* pCommand)
{
	RegionPlayerContext* pPlayerContext = GetPlayerContextBySessionId(pCommand->m_iSessionId);
	if (!pPlayerContext)
	{
		LOG_ERR(LOG_PLAYER, _T("sid=%08x can't find player"), pCommand->m_iSessionId);
		return;
	}

	if (!Receiver::OnPacketReceived((void*)pPlayerContext, pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData))
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x on packet received failed"), pCommand->m_iSessionId);
	}
}

void RegionServerLoop::_OnCommandShutdownPlayerReq(LogicCommandShutdownPlayerReq* pCommand)
{
	LOG_DBG(LOG_PLAYER, _T("name=%s sid=%08x"), pCommand->m_pPlayerContext->m_strAvatarName, pCommand->m_pPlayerContext->m_iSessionId)
	AutoLocker locker(&pCommand->m_pPlayerContext->m_csContext);
	ShutdownPlayer(pCommand->m_pPlayerContext);
}