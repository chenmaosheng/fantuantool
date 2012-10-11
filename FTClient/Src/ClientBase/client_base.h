/*****************************************************************************************
	filename:	client_base.h
	created:	10/03/2012
	author:		chen
	purpose:	basic function of each client

*****************************************************************************************/

#ifndef _H_CLIENT_BASE
#define _H_CLIENT_BASE

#include "client_common.h"
#include "ftd_define.h"
#include "packet.h"
#include "event.h"
#include <list>

class Worker;
class ContextPool;
struct Connector;
class ClientConfig;
class ClientLoop;
struct ClientCommand;
struct ClientEvent;
class ClientBase
{
public:
	friend class ClientLoop;

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
	virtual ~ClientBase();
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

	// connect to login server
	void Login(uint32 iIP, uint16 iPort, const char* strToken);
	// disconnect
	void Logout();

	// handle login packet which is analyzed from received buffer
	int32 HandleLoginPacket(uint16 iLen, char* pBuf);

	// handle server packet which is analyzed from received buffer
	int32 HandlePacket(ServerPacket*);

	ClientEvent* PopClientEvent();

public:
	// receive login ntf from master server
	void LoginNtf(uint32 iGateIP, uint16 iGatePort);
	// receive avatar list from db
	void AvatarListAck(int32 iRet, uint8 iAvatarCount, const ftdAvatar *arrayAvatar);

private:
	// connection handler
	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

private:
	Worker* m_pWorker;
	ContextPool* m_pContextPool;
	Log* m_pLogSystem;
	ClientLoop* m_pMainLoop;

	ConnID m_ConnId;		// connection id
	SOCKADDR_IN m_SockAddr;
	int32 m_iState;	// current state about connect
	uint32 m_iRecvBufLen;	// already received buffer length
	char m_RecvBuf[MAX_OUTPUT_BUFFER];	// the whole received buffer
	bool m_bInLogin;

	TokenPacket m_TokenPacket;
	uint32 m_iGateIP;
	uint16 m_iGatePort;

	std::list<ClientEvent*> m_ClientEventList;
};

extern ClientConfig* g_pClientConfig;
extern ClientBase* g_pClientBase;

#endif
