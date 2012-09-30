/*****************************************************************************************
	filename:	session.h
	created:	09/27/2012
	author:		chen
	purpose:	record connection info and receive event from connection

*****************************************************************************************/

#ifndef _H_SESSION
#define _H_SESSION

#include "server_common.h"
#include "packet.h"
#include "state_machine.h"

// session base state definition
enum
{
	SESSION_STATE_NONE,	// empty state
	SESSION_STATE_ONCONNECTION, // when receive connection
	SESSION_STATE_DISCONNECT, // start disconnect
	SESSION_STATE_ONDISCONNECT, // when received disconnected
	SESSION_STATE_LOGGEDIN, // finish login

	SESSION_STATE_USERDEFINED,
};

// session base event definition
enum
{
	SESSION_EVENT_ONCONNECTION, // when receive connection
	SESSION_EVENT_DISCONNECT, // start disconnect
	SESSION_EVENT_ONDISCONNECT, // when received disconnected
	SESSION_EVENT_ONDATA, // when receive packet
	SESSION_EVENT_SEND, // when send packet
	SESSION_EVENT_LOGGEDIN, // finish login

	SESSION_EVENT_USERDEFINED,
};

#pragma pack(1)
// better way to generate sessionid
union SessionId
{
	struct  
	{
		uint8 sequence_;		// when it's been reused, add 1
		uint16 session_index_;	// session index in session server
		uint8 serverId_;		// session server's id
	}sValue_;

	uint32 iValue_;

	SessionId()
	{
		iValue_ = 0;
	}
};
#pragma pack()

class ServerBase;
struct Connection;
struct ServerPacket;
class Session
{
public:
	Session();
	virtual ~Session();

	virtual void Clear();		// when reuse, clear the session

	// handle event from io operation
	virtual int32 OnConnection(ConnID connId);
	virtual void OnDisconnect();
	virtual void OnData(uint16 iLen, char* pBuf);
	virtual void Disconnect();
	// TypeId means the key point of each packet, the first byte means filterId, the second byte means funcId
	virtual int32 SendData(uint16 iTypeId, uint16 iLen, const char* pData);

	// intialize static session
	static void Initialize(const TCHAR* strPrivateKeyFile, ServerBase* pServer);

protected:
	// handle server packet which is analyzed from received buffer
	int32 HandlePacket(ServerPacket*);
	// handle login packet which is analyzed from received buffer
	int32 HandleLoginPacket(uint16 iLen, char* pBuf);
	// initialize state machine
	virtual void InitStateMachine();
	// notify login success to client
	virtual int32 LoggedInNtf();

private:
	virtual int32 CheckLoginToken(uint16 iLen, char* pBuf) = 0;

public:
	uint32 m_iSessionId;
	Connection* m_pConnection;
	StateMachine m_StateMachine;

	DWORD m_dwConnectionTime;	// onconnection time
	DWORD m_dwLoggedInTime;		// loggedin time

protected:
	uint16 m_iRecvBufLen;
	char m_RecvBuf[MAX_INPUT_BUFFER];	// received buffer from client
	uint16 m_iLoginBufLen;
	TokenPacket m_TokenPacket;

protected:
	static ServerBase* m_pServer;
	// todo: fake secret key issue
	static char* m_pPrivateKey;
	static uint16 m_iPrivateKeyLen;
};
#endif
