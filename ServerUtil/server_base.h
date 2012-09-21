#ifndef _H_SERVER_BASE
#define _H_SERVER_BASE

#include "common.h"

class Worker;
class Acceptor;
class ContextPool;
struct Handler;
class Log;
class LogicLoop;
class ServerBase
{
public:
	virtual int32 Init();
	virtual void Destroy();

	ContextPool* GetContextPool();
	
protected:
	ServerBase();
	virtual ~ServerBase();

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

protected:
	Acceptor* m_pAcceptor;
	Worker* m_pWorker;
	ContextPool* m_pContextPool;

	Log* m_pLogSystem;

public:
	LogicLoop* m_pMainLoop;
};

#endif
