#include "master_server_loop.h"
#include "master_server.h"
#include "master_logic_command.h"

MasterServerLoop::MasterServerLoop()
{
}

MasterServerLoop::~MasterServerLoop()
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
	return true;
}

uint32 MasterServerLoop::_Loop()
{
	return 0;
}

bool MasterServerLoop::_OnCommand(LogicCommand* pCommand)
{
	switch (pCommand->m_iCmdId)
	{
	case 11:
		break;

	default:
		break;
	}

	return true;
}