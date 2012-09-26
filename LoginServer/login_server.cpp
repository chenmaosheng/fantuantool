#include "login_server.h"
#include "login_server_loop.h"
#include "login_packet_dispatch.h"

LoginServer* g_pServer = NULL;

LoginServer::LoginServer()
{
	m_pMasterServer = NULL;
}

int32 LoginServer::Init(const TCHAR* strServerName)
{
	return super::Init(strServerName);
}

void LoginServer::Destroy()
{
	super::Destroy();
}

int32 LoginServer::InitMainLoop()
{
	m_pMasterServer = GetPeerServer((uint16)0);
	if (!m_pMasterServer)
	{
		return -1;
	}

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