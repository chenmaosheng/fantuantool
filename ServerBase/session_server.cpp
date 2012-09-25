#include "session_server.h"
#include "Command.h"
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

void CALLBACK SessionServer::OnData(ConnID connId, uint32 iLen, char* pBuf)
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

int32 SessionServer::Init()
{
	int32 iRet = 0;
	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	iRet = ServerBase::Init();
	if (iRet != 0)
	{
		return -1;
	}

	iRet = InitAcceptor(0, 5151, &handler, 2);
	if (iRet != 0)
	{
		return -2;
	}

	StartAcceptor();

	StartMainLoop();

	return 0;
}

void SessionServer::Destroy()
{
	StopMainLoop();
	DestroyAcceptor();

	ServerBase::Destroy();
}

void SessionServer::Shutdown()
{
	StopAcceptor();

	m_pMainLoop->PushShutdownCommand();

	while (!m_pMainLoop->IsReadyForShutdown())
	{
		Sleep(100);
	}

	m_bReadyForShutdown = true;
}