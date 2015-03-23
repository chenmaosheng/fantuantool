#include "easy_baseserver.h"
// network
#include "easy_net.h"
#include "easy_connection.h"
#include "easy_worker.h"
#include "easy_acceptor.h"
#include "easy_packet.h"
// logic
#include "easy_baseloop.h"
#include "easy_basesession.h"
#include "easy_basecommand.h"
#include "util.h"

BOOL CALLBACK EasyBaseServer::OnConnection(ConnID connId)
{
	EasyBaseServer* pServer = (EasyBaseServer*)((EasyConnection*)connId)->acceptor_->server_;
	LogicCommandOnConnect* pCommand = NULL;

	if (pServer->m_bReadyForShutdown)
	{
		return false;
	}

	if (!pServer->m_pMainLoop)
	{
		return false;
	}

	pCommand = new LogicCommandOnConnect;
	pCommand->m_ConnId = connId;
	pServer->m_pMainLoop->PushCommand(pCommand);
	return true;
}

void CALLBACK EasyBaseServer::OnDisconnect(ConnID connId)
{
	EasyBaseServer* pServer = (EasyBaseServer*)((EasyConnection*)connId)->acceptor_->server_;
	LogicCommandOnDisconnect* pCommand = NULL;

	if (pServer->m_bReadyForShutdown)
	{
		return;
	}

	if (!pServer->m_pMainLoop)
	{
		return;
	}

	pCommand = new LogicCommandOnDisconnect;
	pCommand->m_ConnId = connId;
	pServer->m_pMainLoop->PushCommand(pCommand);
}

void CALLBACK EasyBaseServer::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	EasyBaseServer* pServer = (EasyBaseServer*)((EasyConnection*)connId)->acceptor_->server_;
	LogicCommandOnData* pCommand = NULL;

	if (pServer->m_bReadyForShutdown)
	{
		return;
	}

	if (!pServer->m_pMainLoop)
	{
		return;
	}

	pCommand = new LogicCommandOnData;
	pCommand->m_ConnId = connId;
	if (!pCommand->CopyData(iLen, pBuf))
	{
		SAFE_DELETE(pCommand);
		return;
	}

	pServer->m_pMainLoop->PushCommand(pCommand);
}

void CALLBACK EasyBaseServer::OnConnectFailed(void*)
{
}



EasyBaseServer::EasyBaseServer()
{
	m_pAcceptor = NULL;
	m_pWorker = NULL;
	m_pMainLoop = NULL;
	m_bReadyForShutdown = false;
}

EasyBaseServer::~EasyBaseServer()
{
}

int32 EasyBaseServer::Init()
{
	int32 iRet = 0;

	// set the event handler
	static EasyHandler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;
	
	iRet = InitNet();
	_ASSERT(iRet == 0);
	if (iRet != 0)
	{
		LOG_ERR(_T("Initialize StarNet failed"));
		return -4;
	}

	LOG_STT(_T("Initialize StarNet success"));

	iRet = InitMainLoop();
	_ASSERT(iRet == 0);
	if (iRet != 0)
	{
		LOG_ERR(_T("Initialize main loop failed"));
		return -5;
	}

	LOG_STT(_T("Initialize main loop success"));

	iRet = InitAcceptor(0, 9001, &handler, 2);
	if (iRet != 0)
	{
		LOG_ERR(_T("InitAcceptor failed"));
		return -4;
	}

	LOG_STT(_T("InitAcceptor success"));

	StartMainLoop();
	LOG_STT(_T("Start main loop success"));

	LOG_STT(_T("Initialize success, server is started!"));

	return 0;
}

void EasyBaseServer::Destroy()
{
	LOG_STT(_T("Start to destroy server"));

	StopMainLoop();
	DestroyMainLoop();
	DestroyNet();
}

void EasyBaseServer::Shutdown()
{
	LOG_STT(_T("Ready to shutdown session server"));

	// use command to control shutdown
	m_pMainLoop->PushCommand(new LogicCommandShutdown);

	while (!m_pMainLoop->IsReadyForShutdown())
	{
		Sleep(100);
	}

	m_bReadyForShutdown = true;
}

DWORD EasyBaseServer::GetCurrTime()
{
	return m_pMainLoop->GetCurrTime();
}

DWORD EasyBaseServer::GetDeltaTime()
{
	return m_pMainLoop->GetDeltaTime();
}

int32 EasyBaseServer::InitAcceptor(uint32 ip, uint16 port, EasyHandler *pHandler, uint32 iThreadCount)
{
	// create iocp worker
	m_pWorker = CreateWorker(iThreadCount);
	if (!m_pWorker)
	{
		return -1;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	// create acceptor to receive connection
	m_pAcceptor = CreateAcceptor(&addr, m_pWorker, pHandler);
	if (!m_pAcceptor)
	{
		return -3;
	}

	// bind server and acceptor
	SetServerApp(m_pAcceptor, this);
	
	return 0;
}

void EasyBaseServer::UninitAcceptor()
{
	if (m_pWorker)
	{
		DestroyWorker(m_pWorker);
		m_pWorker = NULL;
	}

	if (m_pAcceptor)
	{
		DestroyAcceptor(m_pAcceptor);
		m_pAcceptor = NULL;
	}
}

int32 EasyBaseServer::StartMainLoop()
{
	if (m_pMainLoop)
	{
		return m_pMainLoop->Start();
	}

	return -1;
}

void EasyBaseServer::StopMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Stop();
		m_pMainLoop->Join();
	}
}

void EasyBaseServer::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}
