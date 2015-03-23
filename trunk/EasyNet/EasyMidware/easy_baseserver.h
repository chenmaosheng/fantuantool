#ifndef _H_EASY_BASESERVER
#define _H_EASY_BASESERVER

#include "common.h"

struct _EasyWorker;
struct _EasyAcceptor;
struct _EasyHandler;
class EasyBaseLoop;
class EasyBaseServer
{
public:
	// initialize server
	virtual int32 Init();
	// destroy server
	virtual void Destroy();
	virtual void Shutdown();

	// time control
	DWORD GetCurrTime();
	DWORD GetDeltaTime();
	
protected:
	EasyBaseServer();
	virtual ~EasyBaseServer();

private:
	// initialize and destroy network host
	int32 _InitAcceptor(uint32 ip, uint16 port, struct _EasyHandler* pHandler, uint32 iThreadCount);
	void _UninitAcceptor();

	void _InitMainLoop();
	void _DestroyMainLoop();

	// start and stop server's main logic loop
	int32 _StartMainLoop();
	void _StopMainLoop();

	virtual EasyBaseLoop* _CreateServerLoop() = 0;

private:
	// handle io event
	static BOOL CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

private:
	struct _EasyAcceptor* m_pAcceptor;
	struct _EasyWorker* m_pWorker;
	
public:
	EasyBaseLoop* m_pMainLoop;
};

#endif
