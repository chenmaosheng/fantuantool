/*****************************************************************************************
	filename:	chat_server.h
	created:	12/13/2014
	author:		chen
	purpose:	create a simple chat server

*****************************************************************************************/

#ifndef _H_CHAT_SERVER
#define _H_CHAT_SERVER

#include "server_base.h"

class ChatServerConfig;
class ChatServer : public ServerBase, public Singleton<ChatServer>
{
public:
	// handle io event
	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32& iLen, char* pBuf, uint32&);
	static void CALLBACK OnConnectFailed(void*);

	ChatServer();
	~ChatServer();

	// intialize session server
	int32 Init(const TCHAR* strServerName);
	// destroy and shutdown session server
	void Destroy();
	void Shutdown();

private:
	int32 InitMainLoop();
	void DestroyMainLoop();

	ServerConfig* CreateConfig(uint32 iRealmId, const TCHAR* strServerName);

public:
	bool m_bReadyForShutdown;
};

extern ChatServer* g_pServer;
extern ChatServerConfig* g_pServerConfig;

#endif
