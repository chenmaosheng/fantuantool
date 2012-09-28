/*****************************************************************************************
	filename:	session_server.h
	created:	09/27/2012
	author:		chen
	purpose:	based on server base, handle one kind of server which need to manage session
				currently is for login server and gate server

*****************************************************************************************/

#ifndef _H_SESSION_SERVER
#define _H_SESSION_SERVER

#include "server_base.h"

class SessionServer : public ServerBase
{
public:
	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

	SessionServer();
	virtual ~SessionServer();

	virtual int32 Init(const TCHAR* strServerName);
	virtual void Destroy();
	virtual void Shutdown();

private:
	virtual int32 GetServerAndPeerConfig(uint32& iPeerIP, uint16& iPeerPort, uint32& iServerIP, uint16& iServerPort, uint32& iThreadCount) = 0;

protected:
	bool m_bReadyForShutdown;
};

#endif
