#include "gate_server_loop.h"
#include "gate_server.h"

GateServerLoop::GateServerLoop()
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
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		_ReadyForShutdown();
	}

	return 100;
}

bool GateServerLoop::_OnCommand(LogicCommand* pCommand)
{
	switch (pCommand->m_iCmdId)
	{
	case 11:
		break;

	default:
		return super::_OnCommand(pCommand);
		break;
	}

	return true;
}