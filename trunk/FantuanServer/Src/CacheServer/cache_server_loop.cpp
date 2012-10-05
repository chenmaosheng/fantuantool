#include "cache_server_loop.h"

CacheServerLoop::CacheServerLoop() :
m_iShutdownStatus(NOT_SHUTDOWN)
{

}

int32 CacheServerLoop::Init()
{
	int32 iRet = 0;

	iRet = super::Init();
	if (iRet != 0)
	{
		return iRet;
	}

	//CachePlayerContext::m_pMainLoop = this;

	return 0;
}

void CacheServerLoop::Destroy()
{
	super::Destroy();
}

int32 CacheServerLoop::Start()
{
	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	return 0;
}

void CacheServerLoop::Stop()
{
	super::Stop();
}

bool CacheServerLoop::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

DWORD CacheServerLoop::_Loop()
{
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		m_iShutdownStatus = READY_FOR_SHUTDOWN;
	}

	return 100;
}

bool CacheServerLoop::_OnCommand(LogicCommand* pCommand)
{
	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return true;
	}

	return true;
}

void CacheServerLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;
}