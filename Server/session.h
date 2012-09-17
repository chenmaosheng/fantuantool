#ifndef _H_SESSION
#define _H_SESSION

#include "common.h"

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
class Session
{
public:
	Session();
	virtual ~Session();

	virtual void Clear();		// when reuse, clear the session

	virtual int32 OnConnection(ConnID connId);
	virtual void OnDisconnect();
	//virtual void OnData(uint16 len, char* buf);
	//virtual void Disconnect();
	// filterId means the key point of each packet, the first byte means todo, the second byte means todo
	//virtual int32 SendData(uint16 filterId, uint16 len, const char* data);

	static void Initialize(ServerBase* pServer);

public:
	uint32 sessionId_;
	ConnID connId_;

protected:
	static ServerBase* server_;
};
#endif
