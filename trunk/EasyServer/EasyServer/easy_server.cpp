#include "easy_server.h"
#include "easy_server_loop.h"

EasyServer* g_pServer = NULL;

int32 EasyServer::InitMainLoop()
{
	m_pMainLoop = new EasyServerLoop;

	return m_pMainLoop->Init(this);
}