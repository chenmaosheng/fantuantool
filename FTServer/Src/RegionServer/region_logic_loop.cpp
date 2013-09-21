#include "region_logic_loop.h"
#include "region_server_loop.h"
#include "region_server.h"
#include "map.h"
#include "map_desc.h"
#include "region_logic_command.h"
#include "region_player_context.h"
#include "avatar.h"
#include "region_server_config.h"

#include "auto_locker.h"

RegionServerLoop* RegionLogicLoop::m_pMainLoop = NULL;

RegionLogicLoop::RegionLogicLoop(uint32 iLoopId)
{
	m_iLoopId = iLoopId;
	m_iShutdownStatus = NOT_SHUTDOWN;
	m_iPlayerCount = 0;
	m_arrayTempAvatar = new Avatar*[g_pServerConfig->m_iPlayerMax];
	m_iTempAvatarCount = 0;

	m_pMainLoop = (RegionServerLoop*)g_pServer->m_pMainLoop;
}

RegionLogicLoop::~RegionLogicLoop()
{
	SAFE_DELETE_ARRAY(m_arrayTempAvatar);
}

int32 RegionLogicLoop::Init(const std::vector<uint16> &vMapId, uint16 iInstanceCount)
{
	int32 iRet = 0;
	Map* pMap = NULL;
	MapDesc* pMapDesc = NULL;

	iRet = super::Init(false);
	if (iRet != 0)
	{
		return iRet;
	}

	// initialize broadcast helper
	for(uint32 i = 0; i < SERVERCOUNT_MAX; ++i)
	{
		m_BroadcastHelper.SetGateServer(i, g_pServer->GetPeerServer(i));
	}

	for (uint32 i = 0; i < vMapId.size(); ++i)
	{
		pMap = Map::Create(vMapId[i]);
		if (!pMap)
		{
			return -1;
		}

		pMap->m_pRegionLogicLoop = this;
		m_mMapById.insert(std::make_pair(vMapId[i], pMap));

		m_pMainLoop->BindNewMap(vMapId[i], pMap);
	}

	return 0;
}

void RegionLogicLoop::Destroy()
{
	super::Destroy();
}

int32 RegionLogicLoop::Start()
{
	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	return 0;
}

void RegionLogicLoop::ShutdownPlayer(RegionPlayerContext* pPlayerContext)
{
}

bool RegionLogicLoop::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

Map* RegionLogicLoop::GetMapById(uint16 iMapId)
{
	stdext::hash_map<uint16, Map*>::iterator mit = m_mMapById.find(iMapId);
	if (mit != m_mMapById.end())
	{
		return mit->second;
	}

	return NULL;
}

uint32 RegionLogicLoop::AddAvatarToTemp(Avatar* pAvatar)
{
	m_arrayTempAvatar[m_iTempAvatarCount++] = pAvatar;
	return m_iTempAvatarCount;
}

void RegionLogicLoop::BroadcastToTemp()
{
	m_BroadcastHelper.Clear();
	for (uint32 i = 0; i < m_iTempAvatarCount; ++i)
	{
		m_BroadcastHelper.AddGateSession(m_arrayTempAvatar[i]->m_pPlayerContext->m_iSessionId);
	}
	m_BroadcastHelper.SendData(m_DelaySendData.m_iDelayTypeId, m_DelaySendData.m_iDelayLen, m_DelaySendData.m_DelayBuf);

	// clear temp array
	m_iTempAvatarCount = 0;
}

void RegionLogicLoop::PushMapEnterCommand(RegionPlayerContext* pPlayerContext, uint16 iMapId)
{
	LogicCommandMapEnterReq* pCommand = FT_NEW(LogicCommandMapEnterReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandMapEnterReq) failed"));
		return;
	}

	pCommand->m_pPlayerContext = pPlayerContext;
	pCommand->m_iMapId = iMapId;
	PushCommand(pCommand);
}

DWORD RegionLogicLoop::_Loop()
{
	return 10;
}

bool RegionLogicLoop::_OnCommand(LogicCommand* pCommand)
{
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_SHUTDOWN:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandShutdown();
		}
		break;

	case COMMAND_PACKETFORWARD:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			//_OnCommandPacketForward((LogicCommandPacketForward*)pCommand);
		}
		break;

	case COMMAND_MAPENTERREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandMapEnterReq((LogicCommandMapEnterReq*)pCommand);
		}
		break;
	}

	return 0;
}

void RegionLogicLoop::_OnCommandShutdown()
{
}

void RegionLogicLoop::_OnCommandMapEnterReq(LogicCommandMapEnterReq* pCommand)
{
	int32 iRet = 0;
	RegionPlayerContext* pPlayerContext = pCommand->m_pPlayerContext;

	LOG_DBG(LOG_PLAYER, _T("name=%s sid=%08x Map enter"), pPlayerContext->m_strAvatarName, pPlayerContext->m_iSessionId);

	AutoLocker locker(&pPlayerContext->m_csContext);

	pPlayerContext->m_iMapId = pCommand->m_iMapId;
	pPlayerContext->m_pMap = GetMapById(pCommand->m_iMapId);
	_ASSERT(pPlayerContext->m_pMap);

	// add new player
	m_iPlayerCount++;

	// avatar enter map
	pPlayerContext->m_pAvatar->OnMapEnterReq(pPlayerContext->m_pMap);
}