#include "easy_server_loop.h"
#include "easy_server.h"
#include "easy_session.h"
#include "easy_logic_command.h"

EasyServerLoop::EasyServerLoop()
{
}

EasyServerLoop::~EasyServerLoop()
{
}

int32 EasyServerLoop::Init()
{
	int32 iRet = 0;

	iRet = LogicLoop::Init(g_pServer);
	if (iRet != 0)
	{
		return iRet;
	}

	EasySession::m_pMainLoop = this;

	return 0;
}

Session* EasyServerLoop::_CreateSession()
{
	return new EasySession;
}

bool EasyServerLoop::_OnCommand(LogicCommand* pCommand)
{
	switch (pCommand->m_iCmdId)
	{
	case COMMAND_ONPINGREQ:
		if ( m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandOnPingReq((LogicCommandOnPingReq*)pCommand);
		}
		break;

	default:
		return LogicLoop::_OnCommand(pCommand);
		break;
	}

	return true;
}

bool EasyServerLoop::_OnCommandOnPingReq(LogicCommandOnPingReq* pCommand)
{
	EasySession* pSession = (EasySession*)_GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		pSession->OnPingReq(pCommand->m_iVersion);
		return true;
	}

	return false;
}