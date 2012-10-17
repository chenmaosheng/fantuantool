#include "server_base.h"
// network
#include "starnet.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "context_pool.h"
#include "packet.h"
// util
#include "log_device_console.h"
#include "log_device_file.h"
#include "minidump.h"
#include "memory_pool.h"
#include "auto_locker.h"
// logic
#include "logic_loop.h"
#include "session.h"
// config
#include "server_config.h"
#include "common_config.h"

ServerBase::ServerBase()
{
	m_pAcceptor = NULL;
	m_pWorker = NULL;
	m_pContextPool= NULL;

	m_pLogSystem = NULL;
	m_pMainLoop = NULL;
	m_pServerConfig = NULL;

	memset(&m_arrayPeerServer, 0, sizeof(m_arrayPeerServer));
}

ServerBase::~ServerBase()
{
}

int32 ServerBase::Init(const TCHAR* strServerName)
{
	int32 iRet = 0;
	
	// initialize minidump
	Minidump::Init(_T("log"));

	// initialize server's configuration
	iRet = InitConfig(strServerName);
	if (iRet != 0)
	{
		_ASSERT(false && _T("Init config error"));
		return -1;
	}

	// initialize log system, after that, log is ready to be used
	iRet = InitLog(m_pServerConfig->GetLogLevel(), m_pServerConfig->GetLogPath(), strServerName, 0);
	if (iRet != 0)
	{
		_ASSERT(false &&_T("Init log error"));
		return -2;
	}

	// set the pairs of log type and log type name
	// todo: later add more pairs
	m_pLogSystem->SetLogTypeString(LOG_NET, _T("StarNet"));
	m_pLogSystem->SetLogTypeString(LOG_SERVER, _T("Server"));
	m_pLogSystem->SetLogTypeString(LOG_PLAYER, _T("Player"));
	m_pLogSystem->SetLogTypeString(LOG_DB, _T("DB"));
	m_pLogSystem->SetLogTypeString(LOG_PROFILER, _T("Profiler"));

	LOG_STT(LOG_SERVER, _T("Initialize log system success"));

	// set the min and max of memory pool object
	MEMORY_POOL_INIT(MEMORY_OBJECT_MIN, MEMORY_OBJECT_MAX);

	LOG_STT(LOG_SERVER, _T("Initialize memory pool success"));

	iRet = StarNet::Init();
	_ASSERT(iRet == 0);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("Initialize StarNet failed"));
		return -3;
	}

	LOG_STT(LOG_SERVER, _T("Initialize StarNet success"));

	iRet = InitMainLoop();
	_ASSERT(iRet == 0);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("Initialize main loop failed"));
		return -4;
	}

	LOG_STT(LOG_SERVER, _T("Initialize main loop success"));
	LOG_STT(LOG_SERVER, _T("Initialize success, server is started!"));

	return 0;
}

void ServerBase::Destroy()
{
	LOG_STT(LOG_SERVER, _T("Start to destroy server"));

	DestroyMainLoop();
	StarNet::Destroy();
	DestroyLog();
	DestroyConfig();
}

ContextPool* ServerBase::GetContextPool()
{
	return m_pContextPool;
}

PEER_SERVER ServerBase::GetPeerServer(uint8 iServerId)
{
	uint32 iIP = 0;
	uint16 iPort = 0;
	ServerConfigItem* pConfigItem = NULL;

	_ASSERT(iServerId < PEER_SERVER_MAX);
	if (iServerId >= PEER_SERVER_MAX)
	{
		LOG_ERR(LOG_SERVER, _T("ServerId is invalid, id=%d"), iServerId);
		return NULL;
	}

	// check if peer server is got before
	if (!m_arrayPeerServer[iServerId])
	{
		pConfigItem = m_pServerConfig->GetServerConfigItemById(iServerId);
		if (pConfigItem)
		{
			iIP = pConfigItem->m_iPeerIP;
			iPort = pConfigItem->m_iPeerPort;
			m_arrayPeerServer[iServerId] = StarNet::GetPeerServer(iIP, iPort);
		}
	}

	return m_arrayPeerServer[iServerId];
}

PEER_SERVER ServerBase::GetPeerServer(const TCHAR* strServerName)
{
	uint32 iIP = 0;
	uint16 iPort = 0;
	ServerConfigItem* pConfigItem = m_pServerConfig->GetServerConfigItem(strServerName);
	_ASSERT(pConfigItem);
	if (pConfigItem)
	{
		iIP = pConfigItem->m_iPeerIP;
		iPort = pConfigItem->m_iPeerPort;
		return StarNet::GetPeerServer(iIP, iPort);
	}

	return NULL;
}

DWORD ServerBase::GetCurrTime()
{
	return m_pMainLoop->GetCurrTime();
}

DWORD ServerBase::GetDeltaTime()
{
	return m_pMainLoop->GetDeltaTime();
}

int32 ServerBase::InitConfig(const TCHAR* strServerName)
{
	uint32 iRealmId = 0;

	m_pServerConfig = CreateConfig(iRealmId, strServerName);
	if (!m_pServerConfig || !m_pServerConfig->LoadConfig())
	{
		return -1;
	}

	return 0;
}

void ServerBase::DestroyConfig()
{
	SAFE_DELETE(m_pServerConfig);
}

int32 ServerBase::InitLog(int32 iLowLogLevel, const TCHAR* strPath, const TCHAR* strLogFileName, uint32 iMaxFileSize)
{
	m_pLogSystem = Log::GetInstance();
	m_pLogSystem->Init(iLowLogLevel);
	
	LogDevice* pDevice = NULL;
	// screen log
	pDevice = m_pLogSystem->CreateAndAddLogDevice(Log::LOG_DEVICE_CONSOLE);
	// file log
	pDevice = m_pLogSystem->CreateAndAddLogDevice(Log::LOG_DEVICE_FILE);
	if (pDevice)
	{
		pDevice->Init(strPath, strLogFileName);
	}

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
	// create iocp worker
	m_pWorker = Worker::CreateWorker(iThreadCount);
	if (!m_pWorker)
	{
		return -1;
	}

	// create pool of context
	m_pContextPool = ContextPool::CreateContextPool(MAX_INPUT_BUFFER, MAX_OUTPUT_BUFFER);
	if (!m_pContextPool)
	{
		return -2;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	// create acceptor to receive connection
	m_pAcceptor = Acceptor::CreateAcceptor(&addr, m_pWorker, m_pContextPool, pHandler);
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

int32 ServerBase::StartPeerServer(uint32 iIP, uint16 iPort)
{
	// only 1 thread is enough, not too many connections
	if (!StarNet::StartPeerServer(iIP, iPort, 1))
	{
		return -1;
	}

	return 0;
}

void ServerBase::StopPeerServer()
{
	StarNet::StopPeerServer();
}














void FuncProfiler::Print()
{
	AutoLocker locker(&PerfCounter::Instance()->m_csPerfCounter);
	LOG_DBG(LOG_PROFILER, _T("%s() time: %.8f count: %d"), m_strFuncName, m_iTimeCost/(double)PerfCounter::Instance()->GetFrequency(), m_iCallCount);
}