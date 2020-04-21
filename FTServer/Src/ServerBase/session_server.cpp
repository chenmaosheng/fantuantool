#include "session_server.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "context_pool.h"
#include "handler.h"
#include "session_server_loop.h"
#include "logic_command.h"
#include "session.h"

bool CALLBACK SessionServer::OnConnection(ConnID connId)
{
	SessionServer* pServer = (SessionServer*)((Connection*)connId)->acceptor_->server_;
	LogicCommandOnConnect* pCommand = NULL;

	if (pServer->m_bReadyForShutdown)
	{
		return false;
	}

	if (!pServer->m_pMainLoop)
	{
		return false;
	}

	pCommand = FT_NEW(LogicCommandOnConnect);
	pCommand->m_ConnId = connId;
	pServer->m_pMainLoop->PushCommand(pCommand);
	return true;
}

void CALLBACK SessionServer::OnDisconnect(ConnID connId)
{
	SessionServer* pServer = (SessionServer*)((Connection*)connId)->acceptor_->server_;
	LogicCommandOnDisconnect* pCommand = NULL;

	if (pServer->m_bReadyForShutdown)
	{
		return;
	}

	if (!pServer->m_pMainLoop)
	{
		return;
	}

	pCommand = FT_NEW(LogicCommandOnDisconnect);
	pCommand->m_ConnId = connId;
	pServer->m_pMainLoop->PushCommand(pCommand);
}

void CALLBACK SessionServer::OnData(ConnID connId, uint32& iLen, char* pBuf, uint32&)
{
	SessionServer* pServer = (SessionServer*)((Connection*)connId)->acceptor_->server_;
	LogicCommandOnData* pCommand = NULL;

	if (pServer->m_bReadyForShutdown)
	{
		return;
	}

	if (!pServer->m_pMainLoop)
	{
		return;
	}

	pCommand = FT_NEW(LogicCommandOnData);
	pCommand->m_ConnId = connId;
	if (!pCommand->CopyData(iLen, pBuf))
	{
		SAFE_DELETE(pCommand);
		return;
	}

	pServer->m_pMainLoop->PushCommand(pCommand);
}

void CALLBACK SessionServer::OnConnectFailed(void*)
{
}

SessionServer::SessionServer() :
m_bReadyForShutdown(false)
{
}

SessionServer::~SessionServer()
{
}

int32 SessionServer::Init(const TCHAR* strServerName)
{
	int32 iRet = 0;
	uint32 iPeerIP = 0;
	uint16 iPeerPort = 0;
	uint32 iServerIP = 0;
	uint16 iServerPort = 0;
	uint32 iThreadCount = 0;

	// set the event handler
	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	iRet = ServerBase::Init(strServerName);
	if (iRet != 0)
	{
		return -1;
	}

	// get all config
	iRet = GetServerAndPeerConfig(iPeerIP, iPeerPort, iServerIP, iServerPort, iThreadCount);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("Get Config failed"));
		return -2;
	}

	// init peer server
	iRet = StartPeerServer(iPeerIP, iPeerPort);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("Start peer server failed, ip=%d, port=%d"), iPeerIP, iPeerPort);
		return -3;
	}

	LOG_STT(LOG_SERVER, _T("StartPeerServer success, IP=%d, port=%d"), iPeerIP, iPeerPort);

	iRet = InitAcceptor(iServerIP, iServerPort, &handler, iThreadCount);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("InitAcceptor failed, ip=%d, port=%d"), iServerIP, iServerPort);
		return -4;
	}

	LOG_STT(LOG_SERVER, _T("InitAcceptor success, IP=%d, port=%d"), iServerIP, iServerPort);

	StartAcceptor();
	LOG_STT(LOG_SERVER, _T("Start acceptor success"));

	StartMainLoop();
	LOG_STT(LOG_SERVER, _T("Start main loop success"));

	return 0;
}

void SessionServer::Destroy()
{
	LOG_STT(LOG_SERVER, _T("Destroy session server"));

	StopMainLoop();
	DestroyAcceptor();
	StopPeerServer();

	ServerBase::Destroy();
}

void SessionServer::Shutdown()
{
	LOG_STT(LOG_SERVER, _T("Ready to shutdown session server"));

	// first stop the acceptor
	StopAcceptor();

	// use command to control shutdown
	m_pMainLoop->PushShutdownCommand();

	while (!m_pMainLoop->IsReadyForShutdown())
	{
		Sleep(100);
	}

	m_bReadyForShutdown = true;
}