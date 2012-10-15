#include "region_server_loop.h"
#include "region_player_context.h"
#include "region_server_config.h"
#include "region_server.h"
#include "region_logic_command.h"

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

DWORD RegionServerLoop::_Loop()
{
	return 10;
}

bool RegionServerLoop::_OnCommand(LogicCommand* pCommand)
{
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_ONREGIONALLOCREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			OnCommandOnRegionAllocReq((LogicCommandOnRegionAllocReq*)pCommand);
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

void RegionServerLoop::OnCommandOnRegionAllocReq(LogicCommandOnRegionAllocReq* pCommand)
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
		ASSERT(false);
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