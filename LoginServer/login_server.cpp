#include "login_server.h"
#include "login_server_loop.h"
#include "login_packet_dispatch.h"

LoginServer* g_pServer = NULL;

LoginServer::LoginServer()
{
}

int32 LoginServer::Init()
{
	return super::Init();
}

void LoginServer::Destroy()
{
	super::Destroy();
}

int32 LoginServer::InitMainLoop()
{
	m_pMainLoop = new LoginServerLoop;

	return m_pMainLoop->Init();
}

void LoginServer::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}

void LoginServer::InitPacketDispatch()
{
	static LoginPacketDispatch _LoginPacketDispatch;
}