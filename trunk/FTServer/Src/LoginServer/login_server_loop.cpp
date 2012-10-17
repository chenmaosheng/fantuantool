#include "login_server_loop.h"
#include "logic_command.h"
#include "login_server.h"
#include "login_server_config.h"
#include "login_logic_command.h"
#include "alarm.h"

LoginServerLoop::LoginServerLoop() :
SessionServerLoop<LoginSession>(g_pServerConfig->m_iSessionMax)
{
}

LoginServerLoop::~LoginServerLoop()
{
}

int32 LoginServerLoop::Init()
{
	PERF_PROFILER;

	int32 iRet = 0;

	iRet = super::Init(g_pServerConfig->m_iServerId, g_pServer);
	if (iRet != 0)
	{
		return iRet;
	}

	LoginSession::m_pMainLoop = this;

	return 0;
}

void LoginServerLoop::Destroy()
{
	super::Destroy();
}

int32 LoginServerLoop::Start()
{
	PERF_PROFILER;

	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	// register alarm event
	m_pAlarm->RegisterEvent(_T("CheckSession"), m_dwCurrTime, 60000, this, &LoginServerLoop::_CheckSessionState);
	m_pAlarm->RegisterEvent(_T("ReportState"), m_dwCurrTime, 60000, this, &LoginServerLoop::_ReportState);

	return 0;
}

DWORD LoginServerLoop::_Loop()
{
	// check if ready for shutdown
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		_ReadyForShutdown();
	}

	return 100;
}

bool LoginServerLoop::_OnCommand(LogicCommand* pCommand)
{
	switch (pCommand->m_iCmdId)
	{
	case COMMAND_ONLOGINFAILEDACK:
		if ( m_iShutdownStatus <= NOT_SHUTDOWN)
		{
			_OnCommandOnLoginFailedAck((LogicCommandOnLoginFailedAck*)pCommand);
		}
		break;

	default:
		return super::_OnCommand(pCommand);
		break;
	}

	return true;
}

bool LoginServerLoop::_OnCommandOnLoginFailedAck(LogicCommandOnLoginFailedAck* pCommand)
{
	LoginSession* pSession = GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		pSession->OnLoginFailedAck(pCommand->m_iReason);
		return true;
	}

	return false;
}

void LoginServerLoop::_CheckSessionState()
{

}

void LoginServerLoop::_ReportState()
{

}