#include "master_server.h"
#include "master_server_loop.h"
#include "master_peer_dispatch.h"

MasterServer* g_pServer = NULL;

MasterServer::MasterServer()
{
}

int32 MasterServer::Init(const TCHAR* strServerName)
{
	int32 iRet = 0;
	iRet = super::Init(strServerName);
	if (iRet != 0)
	{
		return -1;
	}

	iRet = StartPeerServer(INADDR_ANY, server_port[0]);
	if (iRet != 0)
	{
		return -2;
	}

	iRet = StartMainLoop();
	if (iRet != 0)
	{
		return -3;
	}

	return 0;
}

void MasterServer::Destroy()
{
	StopMainLoop();

	StopPeerServer();

	super::Destroy();
}

void MasterServer::Shutdown()
{

}

int32 MasterServer::InitMainLoop()
{
	m_pMainLoop = new MasterServerLoop;

	return m_pMainLoop->Init();
}

void MasterServer::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}

void MasterServer::InitPacketDispatch()
{
	static MasterPeerDispatch _MasterPeerDispatch;
}