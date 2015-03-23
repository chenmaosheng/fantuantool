#ifndef _H_SERVER_BASE
#define _H_SERVER_BASE

#include "common.h"

class Worker;
class Acceptor;
struct Handler;
class LogicLoop;
class ServerBase
{
public:
	// handle io event
	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

	// initialize server
	virtual int32 Init();
	// destroy server
	virtual void Destroy();
	virtual void Shutdown();

	// time control
	DWORD GetCurrTime();
	DWORD GetDeltaTime();
	
protected:
	ServerBase();
	virtual ~ServerBase();

	// initialize and destroy network host
	int32 InitAcceptor(uint32 ip, uint16 port, Handler* pHandler, uint32 iThreadCount);
	void DestroyAcceptor();

	virtual int32 InitMainLoop() = 0;
	virtual void DestroyMainLoop();

	// start and stop network host
	void StartAcceptor();
	void StopAcceptor();
	
	// start and stop server's main logic loop
	int32 StartMainLoop();
	void StopMainLoop();

private:
	Acceptor* m_pAcceptor;
	Worker* m_pWorker;
	bool m_bReadyForShutdown;

public:
	LogicLoop* m_pMainLoop;
};

#endif
