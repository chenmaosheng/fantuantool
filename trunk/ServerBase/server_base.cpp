#include "server_base.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "context_pool.h"
#include "starnet.h"
#include "log_device_console.h"
#include "log_device_file.h"
#include "minidump.h"
#include "logic_loop.h"
#include "singleton.h"
#include "packet.h"
#include "session.h"
#include "memory_pool.h"
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
	
	Minidump::Init(_T("log"));

	iRet = InitConfig(strServerName);
	if (iRet != 0)
	{
		return -1;
	}

	iRet = InitLog(Log::LOG_DEBUG_LEVEL, _T("Log"), _T("Test"), 0);
	if (iRet != 0)
	{
		return -2;
	}

	m_pLogSystem->SetLogTypeString(LOG_STARNET, _T("StarNet"));
	m_pLogSystem->SetLogTypeString(LOG_SERVER, _T("Server"));

	LOG_DBG(LOG_SERVER, _T("Log Loaded"));

	MEMORY_POOL_INIT(8, 65536);

	iRet = StarNet::Init();
	if (iRet != 0)
	{
		return -3;
	}

	iRet = InitMainLoop();
	if (iRet != 0)
	{
		return -4;
	}

	LOG_DBG(LOG_SERVER, _T("StarNet Loaded"));

	LOG_DBG(LOG_SERVER, _T("Server start!"));
	LOG_DBG(LOG_SERVER, _T("Initialize success!"));

	return 0;
}

void ServerBase::Destroy()
{
	DestroyMainLoop();
	StarNet::Destroy();
	DestroyLog();
	DestroyConfig();
}

ContextPool* ServerBase::GetContextPool()
{
	return m_pContextPool;
}

PEER_SERVER ServerBase::GetPeerServer(uint16 iServerId)
{
	uint32 iIP = 0;
	uint16 iPort = 0;
	ServerConfigItem* pConfigItem = NULL;

	if (iServerId >= PEER_SERVER_MAX)
	{
		return NULL;
	}

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
	if (pConfigItem)
	{
		iIP = pConfigItem->m_iPeerIP;
		iPort = pConfigItem->m_iPeerPort;
		return StarNet::GetPeerServer(iIP, iPort);
	}

	return NULL;
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
