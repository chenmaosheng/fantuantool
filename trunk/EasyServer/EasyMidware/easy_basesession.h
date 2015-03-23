#ifndef _H_EASY_BASESESSION
#define _H_EASY_BASESESSION

#include "common.h"

class EasyBaseServer;
struct _EasyPacket;
struct _EasyConnection;
class EasyBaseSession
{
public:
	EasyBaseSession();
	virtual ~EasyBaseSession();

	virtual void Clear();		// when reuse, clear the session

	// handle event from io operation
	virtual int32 OnConnection(ConnID connId);
	virtual void OnDisconnect();
	virtual void OnData(uint16 iLen, char* pBuf);
	virtual void Disconnect();
	virtual int32 SendData(uint16 iTypeId, uint16 iLen, const char* pData);

	// intialize static session
	static int32 Initialize(EasyBaseServer* pServer);

protected:
	// handle server packet which is analyzed from received buffer
	int32 HandlePacket(struct _EasyPacket*);
	
public:
	uint32 m_iSessionId;		// sessionid
	static uint32 m_iSessionIdStart;
	struct _EasyConnection* m_pConnection;	// connection from network
	DWORD m_dwConnectionTime;	// onconnection time
	DWORD m_dwLoggedInTime;		// loggedin time

protected:
	static EasyBaseServer* m_pServer;
	
	uint16 m_iRecvBufLen;		// received buffer length from client
	char m_RecvBuf[MAX_BUFFER];	// received buffer from client
};

#endif
