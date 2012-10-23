/*****************************************************************************************
	filename:	server_base.h
	created:	09/27/2012
	author:		chen
	purpose:	basic function of each server

*****************************************************************************************/

#ifndef _H_SERVER_BASE
#define _H_SERVER_BASE

#include "server_common.h"

class Worker;
class Acceptor;
class ContextPool;
struct Handler;
class Log;
class LogicLoop;
class ServerConfig;
class DataCenter;
class ServerBase
{
public:
	// initialize server
	virtual int32 Init(const TCHAR* strServerName);
	// destroy server
	virtual void Destroy();

	ContextPool* GetContextPool();

	// get peer server by id or name
	PEER_SERVER GetPeerServer(uint8 iServerId);
	PEER_SERVER GetPeerServer(const TCHAR* strServerName);

	// time control
	DWORD GetCurrTime();
	DWORD GetDeltaTime();
	
protected:
	ServerBase();
	virtual ~ServerBase();

	int32 InitConfig(const TCHAR* strServerName);
	void DestroyConfig();
	// new configuration depend on which server
	virtual ServerConfig* CreateConfig(uint32 iRealmId, const TCHAR* strServerName) = 0;

	// initialize and destroy log system
	int32 InitLog(int32 iLowLogLevel, const TCHAR* strPath, const TCHAR* strLogFileName, uint32 iMaxFileSize);
	void DestroyLog();

	// initialize and destroy data
	int32 InitData();
	void DestroyData();

	// initialize and destroy network host
	int32 InitAcceptor(uint32 ip, uint16 port, Handler* pHandler, uint32 iThreadCount);
	void DestroyAcceptor();

	virtual int32 InitMainLoop() = 0;
	virtual void DestroyMainLoop() = 0;

	// start and stop network host
	void StartAcceptor();
	void StopAcceptor();
	
	// start and stop server's main logic loop
	int32 StartMainLoop();
	void StopMainLoop();

	// start and stop server in peer mode
	int32 StartPeerServer(uint32 iIP, uint16 iPort);
	void StopPeerServer();

private:
	Acceptor* m_pAcceptor;
	Worker* m_pWorker;
	ContextPool* m_pContextPool;
	PEER_SERVER m_arrayPeerServer[SERVERCOUNT_MAX];

	ServerConfig* m_pServerConfig;
	Log* m_pLogSystem;
	DataCenter* m_pDataCenter;

public:
	LogicLoop* m_pMainLoop;
};

#endif
