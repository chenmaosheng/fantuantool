#include "server_base.h"
// network
#include "network.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "packet.h"
// logic
#include "logic_loop.h"
#include "session.h"
#include "logic_command.h"
#include "util.h"

bool CALLBACK ServerBase::OnConnection(ConnID connId)
{
	ServerBase* pServer = (ServerBase*)((Connection*)connId)->acceptor_->server_;
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

void CALLBACK ServerBase::OnDisconnect(ConnID connId)
{
	ServerBase* pServer = (ServerBase*)((Connection*)connId)->acceptor_->server_;
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

void CALLBACK ServerBase::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	ServerBase* pServer = (ServerBase*)((Connection*)connId)->acceptor_->server_;
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

void CALLBACK ServerBase::OnConnectFailed(void*)
{
}



ServerBase::ServerBase()
{
	m_pAcceptor = NULL;
	m_pWorker = NULL;
	m_pMainLoop = NULL;
	m_bReadyForShutdown = false;
}

ServerBase::~ServerBase()
{
}

int32 ServerBase::Init()
{
	int32 iRet = 0;

	// set the event handler
	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;
	
	iRet = Network::Init();
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

	StartAcceptor();
	LOG_STT(_T("Start acceptor success"));

	StartMainLoop();
	LOG_STT(_T("Start main loop success"));

	LOG_STT(_T("Initialize success, server is started!"));

	return 0;
}

void ServerBase::Destroy()
{
	LOG_STT(_T("Start to destroy server"));

	StopMainLoop();
	DestroyMainLoop();
	Network::Destroy();
}

void ServerBase::Shutdown()
{
	LOG_STT(_T("Ready to shutdown session server"));

	// first stop the acceptor
	StopAcceptor();

	// use command to control shutdown
	m_pMainLoop->PushCommand(new LogicCommandShutdown);

	while (!m_pMainLoop->IsReadyForShutdown())
	{
		Sleep(100);
	}

	m_bReadyForShutdown = true;
}

DWORD ServerBase::GetCurrTime()
{
	return m_pMainLoop->GetCurrTime();
}

DWORD ServerBase::GetDeltaTime()
{
	return m_pMainLoop->GetDeltaTime();
}

int32 ServerBase::InitAcceptor(uint32 ip, uint16 port, Handler *pHandler, uint32 iThreadCount)
{
	// create iocp worker
	m_pWorker = Worker::CreateWorker(iThreadCount);
	if (!m_pWorker)
	{
		return -1;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	// create acceptor to receive connection
	m_pAcceptor = Acceptor::CreateAcceptor(&addr, m_pWorker, pHandler);
	if (!m_pAcceptor)
	{
		return -3;
	}

	// bind server and acceptor
	m_pAcceptor->SetServer(this);

	return 0;
}

void ServerBase::DestroyAcceptor()
{
	if (m_pWorker)
	{
		Worker::DestroyWorker(m_pWorker);
		m_pWorker = NULL;
	}

	if (m_pAcceptor)
	{
		Acceptor::DestroyAcceptor(m_pAcceptor);
		m_pAcceptor = NULL;
	}
}

void ServerBase::StartAcceptor()
{
	m_pAcceptor->Start();
}

void ServerBase::StopAcceptor()
{
	m_pAcceptor->Stop();
}

int32 ServerBase::StartMainLoop()
{
	if (m_pMainLoop)
	{
		return m_pMainLoop->Start();
	}

	return -1;
}

void ServerBase::StopMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Stop();
		m_pMainLoop->Join();
	}
}

void ServerBase::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}
