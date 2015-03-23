#include "easy_server.h"
#include "easy_server_loop.h"

EasyServer* g_pServer = NULL;

EasyBaseLoop* EasyServer::_CreateServerLoop()
{
	return new EasyServerLoop;
}