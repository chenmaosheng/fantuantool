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
class ServerBase
{
public:
	virtual int32 Init(const TCHAR* strServerName);
	virtual void Destroy();

	ContextPool* GetContextPool();
	PEER_SERVER GetPeerServer(uint16 iServerId);
	PEER_SERVER GetPeerServer(const TCHAR* strServerName);
	
protected:
	ServerBase();
	virtual ~ServerBase();

	int32 InitConfig(const TCHAR* strServerName);
	void DestroyConfig();
	virtual ServerConfig* CreateConfig(uint32 iRealmId, const TCHAR* strServerName) = 0;

	int32 InitLog(int32 iLowLogLevel, const TCHAR* strPath, const TCHAR* strLogFileName, uint32 iMaxFileSize);
	void DestroyLog();

	int32 InitAcceptor(uint32 ip, uint16 port, Handler* pHandler, uint32 iThreadCount);
	void DestroyAcceptor();

	virtual int32 InitMainLoop() = 0;
	virtual void DestroyMainLoop() = 0;

	void StartAcceptor();
	void StopAcceptor();
	
	int32 StartMainLoop();
	void StopMainLoop();

	int32 StartPeerServer(uint32 iIP, uint16 iPort);
	void StopPeerServer();

private:
	Acceptor* m_pAcceptor;
	Worker* m_pWorker;
	ContextPool* m_pContextPool;
	PEER_SERVER m_arrayPeerServer[PEER_SERVER_MAX];

	ServerConfig* m_pServerConfig;
	Log* m_pLogSystem;

public:
	LogicLoop* m_pMainLoop;
};

#endif
