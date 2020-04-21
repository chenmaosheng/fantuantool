#include "chat_server.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "context_pool.h"
#include "handler.h"
#include "chat_server_loop.h"
#include "logic_command.h"
#include "chat_session.h"
#include "chat_server_config.h"

ChatServer* g_pServer = NULL;
ChatServerConfig* g_pServerConfig = NULL;

bool CALLBACK ChatServer::OnConnection(ConnID connId)
{
	ChatServer* pServer = (ChatServer*)((Connection*)connId)->acceptor_->server_;
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

void CALLBACK ChatServer::OnDisconnect(ConnID connId)
{
	ChatServer* pServer = (ChatServer*)((Connection*)connId)->acceptor_->server_;
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

void CALLBACK ChatServer::OnData(ConnID connId, uint32& iLen, char* pBuf, uint32&)
{
	ChatServer* pServer = (ChatServer*)((Connection*)connId)->acceptor_->server_;
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

void CALLBACK ChatServer::OnConnectFailed(void*)
{
}

ChatServer::ChatServer() :
m_bReadyForShutdown(false)
{
}

ChatServer::~ChatServer()
{
}

int32 ChatServer::Init(const TCHAR* strServerName)
{
	int32 iRet = 0;
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

	iRet = InitAcceptor(g_pServerConfig->m_iServerIP, g_pServerConfig->m_iServerPort, &handler, g_pServerConfig->m_iThreadCount);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("InitAcceptor failed, ip=%d, port=%d"), g_pServerConfig->m_iServerIP, g_pServerConfig->m_iServerPort);
		return -4;
	}

	LOG_STT(LOG_SERVER, _T("InitAcceptor success, IP=%d, port=%d"), g_pServerConfig->m_iServerIP, g_pServerConfig->m_iServerPort);

	StartAcceptor();
	LOG_STT(LOG_SERVER, _T("Start acceptor success"));

	StartMainLoop();
	LOG_STT(LOG_SERVER, _T("Start main loop success"));

	return 0;
}

void ChatServer::Destroy()
{
	LOG_STT(LOG_SERVER, _T("Destroy session server"));

	StopMainLoop();
	DestroyAcceptor();
	StopPeerServer();

	ServerBase::Destroy();
}

ServerConfig* ChatServer::CreateConfig(uint32, const TCHAR* strServerName)
{
	g_pServerConfig = new ChatServerConfig(strServerName);
	return g_pServerConfig;
}

void ChatServer::Shutdown()
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

int32 ChatServer::InitMainLoop()
{
	m_pMainLoop = new ChatServerLoop;

	return m_pMainLoop->Init();
}

void ChatServer::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}