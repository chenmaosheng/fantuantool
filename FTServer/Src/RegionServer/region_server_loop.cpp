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

void RegionServerLoop::OnCommandOnRegionAllocReq(LogicCommandOnRegionAllocReq* pCommand)
{
	// todo:
}