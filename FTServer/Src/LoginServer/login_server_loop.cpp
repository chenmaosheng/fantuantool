#include "login_server_loop.h"
#include "logic_command.h"
#include "login_server.h"
#include "login_server_config.h"
#include "login_logic_command.h"
#include "alarm.h"

#include "master_peer_send.h"

LoginServerLoop::LoginServerLoop() :
SessionServerLoop<LoginSession>(g_pServerConfig->m_iSessionMax)
{
}

LoginServerLoop::~LoginServerLoop()
{
}

int32 LoginServerLoop::Init()
{
	//PERF_PROFILER;

	int32 iRet = 0;

	iRet = super::Init(g_pServerConfig->m_iServerId, g_pServerConfig->m_strPrivateKey, g_pServer);
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
	//PERF_PROFILER;

	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	// register alarm event
	m_pAlarm->RegisterEvent(_T("CheckSession"), m_dwCurrTime, g_pServerConfig->m_iCheckSessionInterval, this, &LoginServerLoop::_CheckSessionState);
	m_pAlarm->RegisterEvent(_T("ReportState"), m_dwCurrTime, g_pServerConfig->m_iReportInterval, this, &LoginServerLoop::_ReportState);

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
		if ( m_iShutdownStatus < START_SHUTDOWN)
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
	LoginSession* pSession = NULL;
	DWORD dwCurrTime = GetCurrTime();

	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return;
	}

	for (uint16 i = 0; i < m_iSessionMax; ++i)
	{
		pSession = m_arraySession[i];
		if (pSession->m_StateMachine.GetCurrState() == SESSION_STATE_NONE)
		{
			continue;
		}

		switch(pSession->m_StateMachine.GetCurrState())
		{
		case SESSION_STATE_ONCONNECTION:
			{
				if (pSession->m_dwConnectionTime + g_pServerConfig->m_iConnectionTimeout <= dwCurrTime)
				{
					LOG_DBG(LOG_SERVER, _T("sid=%08x Connection timeout"), pSession->m_iSessionId);
					pSession->Disconnect();
				}
			}
			break;

		case SESSION_STATE_LOGGEDIN:
			{
				if (pSession->m_dwLoggedInTime + g_pServerConfig->m_iLoginTimeout <= dwCurrTime)
				{
					LOG_DBG(LOG_SERVER, _T("sid=%08x Login timeout"), pSession->m_iSessionId);
					pSession->Disconnect();
				}
			}
			break;

		case SESSION_STATE_ONVERSIONREQ:
			{
				// todo:
			}
			break;
		}
	}
}

void LoginServerLoop::_ReportState()
{
	int32 iRet = 0;

	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return;
	}

	iRet = MasterPeerSend::LoginReportState(g_pServer->m_pMasterServer, g_pServerConfig->m_iServerId);
	if (iRet != 0)
	{
		_ASSERT(false && _T("LoginReportState failed"));
		LOG_ERR(LOG_SERVER, _T("LoginReportState failed"));
	}
}