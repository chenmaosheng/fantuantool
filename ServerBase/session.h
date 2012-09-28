/*****************************************************************************************
	filename:	session.h
	created:	09/27/2012
	author:		chen
	purpose:	record connection info and receive event from connection

*****************************************************************************************/

#ifndef _H_SESSION
#define _H_SESSION

#include "server_common.h"

#pragma pack(1)
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

	virtual int32 OnConnection(ConnID connId);
	virtual void OnDisconnect();
	virtual void OnData(uint16 iLen, char* pBuf);
	virtual void Disconnect();
	// TypeId means the key point of each packet, the first byte means filterId, the second byte means funcId
	virtual int32 SendData(uint16 iTypeId, uint16 iLen, const char* pData);

	static void Initialize(ServerBase* pServer);

protected:
	int32 HandlePacket(ServerPacket*);
	void SaveSendData(uint16 iTypeId, uint16 iLen, char* pBuf);

public:
	uint32 m_iSessionId;
	Connection* m_pConnection;

protected:
	uint16 m_iRecvBufLen;
	char m_RecvBuf[MAX_INPUT_BUFFER];

protected:
	static ServerBase* m_pServer;
};
#endif
