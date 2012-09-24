#include "master_server.h"
#include "master_server_loop.h"

MasterServer* g_pServer = NULL;

MasterServer::MasterServer()
{
}

int32 MasterServer::Init()
{
	return super::Init();
}

void MasterServer::Destroy()
{
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
}