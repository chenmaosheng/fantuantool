#ifndef _H_EASY_CLIENT
#define _H_EASY_CLIENT

#include "common.h"
#include "easy_packet.h"
#include <list>
#include <hash_map>

struct _EasyWorker;
class EasyClientLoop;
struct LogicCommand;
struct EasyEvent;
class EasyClient
{
public:
	friend class EasyClientLoop;

	enum
	{
		NOT_CONNECT,
		CONNECTING,
		CONNECTED,
		CONNECT_FAILED,
		DISCONNECTED,
		DESTORYING,
		LOGGEDIN,
	};

	EasyClient();
	~EasyClient();
	void Clear();

	// initialize client
	virtual int32 Init();
	// destroy client
	virtual void Destroy();
	virtual bool OnClientConnection(ConnID connId);
	virtual void OnClientDisconnect(ConnID connId);
	// receive data
	virtual void OnClientData(uint32 iLen, char* pBuf);
	// send data
	virtual void SendData(uint16 iTypeId, uint16 iLen, const char* pData);
	// handle server packet which is analyzed from received buffer
	int32 HandlePacket(EasyPacket*);
	// pop a event from event list;
	EasyEvent* PopClientEvent();

	// connect to login server
	void Login(uint32 iIP, uint16 iPort);
	// disconnect
	void Logout();
		
private:
	// connection handler
	static BOOL CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

private:
	struct _EasyWorker* m_pWorker;
	EasyClientLoop* m_pMainLoop;

	ConnID m_ConnId;                // connection id
	SOCKADDR_IN m_SockAddr;
	int32 m_iState; // current state about connect
	uint32 m_iRecvBufLen;   // already received buffer length
	char m_RecvBuf[MAX_BUFFER];      // the whole received buffer
        
	std::list<EasyEvent*> m_ClientEventList;
	CRITICAL_SECTION m_csClientEvent;
};

extern EasyClient* g_pClient;

#endif
