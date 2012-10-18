#include "region_server_loop.h"
#include "region_player_context.h"
#include "region_server_config.h"
#include "region_server.h"
#include "region_logic_command.h"

#include "session.h"

GateServerContext::GateServerContext()
{
	// todo:
	m_arrayPlayerContext = new RegionPlayerContext*[1000];
	memset(m_arrayPlayerContext, 0, sizeof(RegionPlayerContext*)*1000);
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
}

int32 RegionServerLoop::Init()
{
	int32 iRet = 0;
	
	iRet = super::Init();
	if (iRet != 0)
	{
		return iRet;
	}

	RegionPlayerContext::m_pMainLoop = this;

	// initialize broadcast helper
	for(uint32 i = 0; i < SERVERCOUNT_MAX; ++i)
	{
		m_BroadcastHelper.SetGateServer(i, g_pServer->GetPeerServer(i));
	}
	
	return 0;
}

void RegionServerLoop::Destroy()
{
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
		// can't garantee two avatars with same charid have same sessionid
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
		_ASSERT(false);
		LOG_ERR(LOG_PLAYER, _T("sid=%08x session index is allocated"), iSessionId);
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
		_ASSERT(false);
		LOG_ERR(LOG_PLAYER, _T("sid=%08x serverId=%d or session index=%d invalid"), pCommand->m_iSessionId, iServerId, iSessionIndex);
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