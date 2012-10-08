/*****************************************************************************************
	filename:	client_base.h
	created:	10/03/2012
	author:		chen
	purpose:	basic function of each client

*****************************************************************************************/

#ifndef _H_CLIENT_BASE
#define _H_CLIENT_BASE

#include "client_common.h"
#include "packet.h"
#include "event.h"

class Worker;
struct Connector;
class ClientConfig;
class ClientBase
{
public:
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

	ClientBase();
	~ClientBase();

	/////////////////////////event definition//////////////////////////////
	typedef Event<void> DisconnectEvent;
	void SetDisconnectEvent(DisconnectEvent* pDisconnectEvent) { m_pDisconnectEvent = pDisconnectEvent; }

	//////////////////////////////////////////////////////////////////////////

	// initialize client
	virtual int32 Init();
	// destroy client
	virtual void Destroy();
	// receive data
	virtual void OnClientData(uint32 iLen, char* pBuf);
	// send data
	virtual void SendData(uint16 iTypeId, uint16 iLen, const char* pData);

	// connect to login server
	void Login(uint32 iIP, uint16 iPort, const char* strToken);

	// handle login packet which is analyzed from received buffer
	int32 HandleLoginPacket(uint16 iLen, char* pBuf);

	// handle server packet which is analyzed from received buffer
	int32 HandlePacket(ServerPacket*);

public:
	// receive login ntf from master server
	void LoginNtf(uint32 iGateIP, uint16 iGatePort);

private:
	// connection handler
	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

private:
	Worker* m_pWorker;
	Log* m_pLogSystem;
	Connector* m_pConnector;
	SOCKADDR_IN m_SockAddr;
	int32 m_iState;	// current state about connect
	uint32 m_iRecvBufLen;	// already received buffer length
	char m_RecvBuf[MAX_OUTPUT_BUFFER];	// the whole received buffer
	bool m_bInLogin;

	TokenPacket m_TokenPacket;

private:
	// define event
	DisconnectEvent* m_pDisconnectEvent;
};

extern ClientConfig* g_pClientConfig;

#endif
