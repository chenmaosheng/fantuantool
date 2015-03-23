#ifndef _H_SESSION
#define _H_SESSION

#include "common.h"
#include "packet.h"

class ServerBase;
struct Connection;
struct Packet;
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
	virtual int32 SendData(uint16 iTypeId, uint16 iLen, const char* pData);

	// intialize static session
	static int32 Initialize(ServerBase* pServer);

protected:
	// handle server packet which is analyzed from received buffer
	int32 HandlePacket(Packet*);
	
public:
	uint32 m_iSessionId;		// sessionid
	static uint32 m_iSessionIdStart;
	Connection* m_pConnection;	// connection from network
	DWORD m_dwConnectionTime;	// onconnection time
	DWORD m_dwLoggedInTime;		// loggedin time

protected:
	static ServerBase* m_pServer;
	
	uint16 m_iRecvBufLen;		// received buffer length from client
	char m_RecvBuf[MAX_BUFFER];	// received buffer from client
};

#endif
