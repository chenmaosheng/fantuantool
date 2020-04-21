/*****************************************************************************************
	filename:	chat_session.h
	created:	12/13/2014
	author:		chen
	purpose:	record connection info and receive event from connection

*****************************************************************************************/

#ifndef _H_CHAT_SESSION
#define _H_CHAT_SESSION

#include "server_common.h"
#include "state_machine.h"
#include "packet.h"
#include "..\openssl\des.h"

// session base state definition
enum
{
	SESSION_STATE_NONE,	// empty state
	SESSION_STATE_ONCONNECTION, // when receive connection
	SESSION_STATE_DISCONNECT, // start disconnect
	SESSION_STATE_ONDISCONNECT, // when received disconnected
	SESSION_STATE_LOGGEDIN, // finish login
	SESSION_STATE_ONLOGINREQ,	// receive login req
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
	SESSION_EVENT_ONLOGINREQ,
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
class ChatServerLoop;
class ChatSession
{
public:
	ChatSession();
	~ChatSession();

	void Clear();		// when reuse, clear the session

	// handle event from io operation
	int32 OnConnection(ConnID connId);
	void OnDisconnect();
	void OnData(uint16 iLen, char* pBuf);
	void Disconnect();
	// TypeId means the key point of each packet, the first byte means filterId, the second byte means funcId
	int32 SendData(uint16 iTypeId, uint16 iLen, const char* pData);

	static int32 Initialize(ServerBase* pServer);

protected:
	// handle server packet which is analyzed from received buffer
	int32 HandlePacket(ServerPacket*);
	// handle login packet which is analyzed from received buffer
	int32 HandleLoginPacket(uint16 iLen, char* pBuf);
	// initialize state machine
	void InitStateMachine();
	// check token
	int32 CheckLoginToken(uint16 iLen, char* pBuf);
	// notify login success to client
	int32 LoggedInNtf();

public:
	uint32 m_iSessionId;			// preallocated sessionid
	Connection* m_pConnection;		// connection from network
	StateMachine m_StateMachine;	// session's fsm

	DWORD m_dwConnectionTime;	// onconnection time
	DWORD m_dwLoggedInTime;		// loggedin time

protected:
	uint16 m_iRecvBufLen;				// received buffer length from client
	char m_RecvBuf[MAX_INPUT_BUFFER];	// received buffer from client
	uint16 m_iLoginBufLen;

	static ServerBase* m_pServer;

public:
	static ChatServerLoop* m_pMainLoop;

private:
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

#endif
