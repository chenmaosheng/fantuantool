/*****************************************************************************************
	filename:	server_base.h
	created:	12/12/2014
	author:		chen
	purpose:	very simple server

*****************************************************************************************/

#ifndef _H_SIMPLE_SERVER
#define _H_SIMPLE_SERVER

#include "server_common.h"

class Worker;
class Acceptor;
class ContextPool;
struct Handler;
class Log;
class LogicLoop;
class ServerConfig;
class DataCenter;
class SimpleServer
{
public:
	// initialize server
	virtual int32 Init();
	// destroy server
	virtual void Destroy();

	ContextPool* GetContextPool();

	// time control
	DWORD GetCurrTime();
	DWORD GetDeltaTime();
	
protected:
	SimpleServer();
	~SimpleServer();

	int32 InitConfig();
	void DestroyConfig();
	
	// initialize and destroy log system
	int32 InitLog(int32 iLowLogLevel, const TCHAR* strPath, const TCHAR* strLogFileName, uint32 iMaxFileSize);
	void DestroyLog();

	// initialize and destroy network host
	int32 InitAcceptor(uint32 ip, uint16 port, Handler* pHandler, uint32 iThreadCount);
	void DestroyAcceptor();

	int32 InitMainLoop();
	void DestroyMainLoop();

	// start and stop network host
	void StartAcceptor();
	void StopAcceptor();
	
	// start and stop server's main logic loop
	int32 StartMainLoop();
	void StopMainLoop();

private:
	Acceptor* m_pAcceptor;
	Worker* m_pWorker;
	ContextPool* m_pContextPool;
	
	ServerConfig* m_pServerConfig;
	Log* m_pLogSystem;

protected:
	DataCenter* m_pDataCenter;

public:
	LogicLoop* m_pMainLoop;
};

#endif
