#include "gate_server_loop.h"
#include "gate_server.h"
#include "gate_logic_command.h"
#include "gate_server_config.h"

GateServerLoop::GateServerLoop() :
SessionServerLoop<GateSession>(g_pServerConfig->m_iSessionMax)
{
}

GateServerLoop::~GateServerLoop()
{
}

int32 GateServerLoop::Init()
{
	int32 iRet = 0;

	iRet = super::Init(g_pServer);
	if (iRet != 0)
	{
		return iRet;
	}

	return 0;
}

void GateServerLoop::Destroy()
{
	super::Destroy();
}

int32 GateServerLoop::Start()
{
	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	return 0;
}

uint32 GateServerLoop::_Loop()
{
	//// check if ready for shutdown
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		_ReadyForShutdown();
	}

	return 100;
}

bool GateServerLoop::_OnCommand(LogicCommand* pCommand)
{
	// check if ready for shutdown, then ignore all the commands
	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return true;
	}

	switch (pCommand->m_iCmdId)
	{
	case COMMAND_GATEHOLDREQ:
		_OnCommandGateHoldReq((LogicCommandGateHoldReq*)pCommand);
		break;

	default:
		return super::_OnCommand(pCommand);
		break;
	}

	return true;
}

void GateServerLoop::_OnCommandGateHoldReq(LogicCommandGateHoldReq* pCommand)
{
	stdext::hash_map<std::wstring, GateSession*>::iterator mit = m_mSessionMapByName.find(pCommand->m_strAccountName);
	if (mit != m_mSessionMapByName.end())
	{
		LOG_ERR(LOG_SERVER, _T("Find a duplicate account on server, account=%s"), pCommand->m_strAccountName);
		return;
	}

	GateSession* pSession = m_SessionPool.Allocate();
	m_mSessionMapByName.insert(std::make_pair(pCommand->m_strAccountName, pSession));
	pSession->OnHoldReq(pCommand->m_iLoginSessionId, pCommand->m_strAccountName);
}