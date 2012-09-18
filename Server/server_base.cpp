#include "server_base.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "context_pool.h"
#include "starnet.h"
#include "log.h"
#include "log_device_console.h"
#include "log_device_file.h"

ServerBase::ServerBase()
{
	m_pAcceptor = NULL;
	m_pWorker = NULL;
	m_pContextPool= NULL;

	m_pLogSystem = NULL;
}

ServerBase::~ServerBase()
{
}

int32 ServerBase::Init()
{
	int32 iRet = 0;
	
	iRet = InitLog(Log::LOG_DEBUG_LEVEL, 1, _T("Log"), _T("Test"), 0);
	if (iRet != 0)
	{
		return -2;
	}

	LOG_DBG(_T("Server"), _T("Log Loaded"));

	iRet = StarNet::Init();
	if (iRet != 0)
	{
		return -1;
	}

	LOG_DBG(_T("Server"), _T("StarNet Loaded"));

	LOG_DBG(_T("Server"), _T("Server start!"));
	LOG_DBG(_T("Server"), _T("Initialize success!"));

	return 0;
}

void ServerBase::Destroy()
{
	StarNet::Destroy();
}

int32 ServerBase::InitLog(int32 iLowLogLevel, int32 iLogTypeMask, const TCHAR* strPath, const TCHAR* strLogFileName, uint32 iMaxFileSize)
{
	m_pLogSystem = Log::Instance();
	m_pLogSystem->Init(iLowLogLevel, iLogTypeMask);
	
	// screen log
	m_pLogSystem->AddLogDevice(new LogDeviceConsole);

	// file log
	m_pLogSystem->AddLogDevice(new LogDeviceFile(strPath, strLogFileName));

	// start log system
	m_pLogSystem->Start();

	return 0;
}

void ServerBase::DestroyLog()
{
	m_pLogSystem->Destroy();
}

int32 ServerBase::InitAcceptor(uint32 ip, uint16 port, Handler *pHandler, uint32 iThreadCount)
{
	m_pWorker = Worker::CreateWorker(iThreadCount);
	if (!m_pWorker)
	{
		return -1;
	}

	m_pContextPool = ContextPool::CreateContextPool(MAX_INPUT_BUFFER, MAX_OUTPUT_BUFFER);
	if (!m_pContextPool)
	{
		return -2;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	m_pAcceptor = Acceptor::CreateAcceptor(&addr, m_pWorker, m_pContextPool, pHandler);
	if (!m_pAcceptor)
	{
		return -3;
	}

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

	if (m_pContextPool)
	{
		ContextPool::DestroyContextPool(m_pContextPool);
		m_pContextPool = NULL;
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
}

ContextPool* ServerBase::GetContextPool()
{
	return m_pContextPool;
}
