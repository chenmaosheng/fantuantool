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
	EasySession::m_pMainLoop = this;

	return 0;
}

EasyBaseSession* EasyServerLoop::_CreateSession()
{
	return new EasySession;
}
