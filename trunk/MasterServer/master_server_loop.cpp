#include "master_server_loop.h"
#include "master_server.h"
#include "master_logic_command.h"
#include "master_player_context.h"

MasterServerLoop::MasterServerLoop() :
m_iShutdownStatus(NOT_SHUTDOWN),
m_PlayerContextPool(0)
{
	
}

int32 MasterServerLoop::Init()
{
	int32 iRet = 0;

	iRet = super::Init();
	if (iRet != 0)
	{
		return iRet;
	}

	MasterPlayerContext::m_pMainLoop = this;

	return 0;
}

void MasterServerLoop::Destroy()
{
	super::Destroy();
}

int32 MasterServerLoop::Start()
{
	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	return 0;
}

bool MasterServerLoop::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

uint32 MasterServerLoop::_Loop()
{
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		if (m_mPlayerContextBySessionId.empty())
		{
			m_iShutdownStatus = READY_FOR_SHUTDOWN;
		}
	}

	return 100;
}

void MasterServerLoop::_ShutdownPlayer(MasterPlayerContext*)
{
}

bool MasterServerLoop::_OnCommand(LogicCommand* pCommand)
{
	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return true;
	}

	switch (pCommand->m_iCmdId)
	{
	case COMMAND_SHUTDOWN:
		_OnCommandShutdown();
		break;

	default:
		break;
	}

	return true;
}

void MasterServerLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;

	for (stdext::hash_map<const TCHAR*, MasterPlayerContext*>::iterator mit = m_mPlayerContextByName.begin();
		mit != m_mPlayerContextByName.end(); ++mit)
	{
		_ShutdownPlayer(mit->second);
	}
}